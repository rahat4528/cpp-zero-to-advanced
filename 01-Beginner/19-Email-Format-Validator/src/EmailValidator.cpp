#include "EmailValidator.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace email_validator {

// Allowed characters in local-part per RFC 5322 (unquoted)
static bool isLocalChar(char c) {
    if (std::isalnum(static_cast<unsigned char>(c))) return true;
    const std::string allowed = "!#$%&'*+/=?^_`{|}~-";
    return allowed.find(c) != std::string::npos;
}

static bool isDomainChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '-';
}

ValidationResult EmailValidator::validate(const std::string& email) {
    ValidationResult res;
    res.email = email;

    if (email.empty()) {
        res.failureReason = "Email address cannot be empty.";
        return res;
    }

    if (email.length() > 254) {
        res.failureReason = "Total email length exceeds RFC 5321 maximum of 254 characters.";
        return res;
    }

    // Split at the LAST @ symbol (in case of quoted @ in local part)
    size_t atPos = email.rfind('@');
    if (atPos == std::string::npos) {
        res.failureReason = "Missing '@' separator.";
        return res;
    }

    if (atPos == 0) {
        res.failureReason = "Local part (username) is missing before '@'.";
        return res;
    }

    if (atPos == email.length() - 1) {
        res.failureReason = "Domain part is missing after '@'.";
        return res;
    }

    res.localPart = email.substr(0, atPos);
    res.domainPart = email.substr(atPos + 1);

    // 1. Local part validation
    if (res.localPart.length() > 64) {
        res.failureReason = "Local part exceeds RFC 5321 limit of 64 characters.";
        return res;
    }

    if (res.localPart.front() == '.' || res.localPart.back() == '.') {
        res.failureReason = "Local part cannot begin or end with a period ('.').";
        return res;
    }

    bool lastWasDot = false;
    for (char c : res.localPart) {
        if (c == '.') {
            if (lastWasDot) {
                res.failureReason = "Local part cannot contain consecutive periods ('..').";
                return res;
            }
            lastWasDot = true;
        } else {
            lastWasDot = false;
            if (!isLocalChar(c)) {
                res.failureReason = std::string("Local part contains invalid character: '") + c + "'.";
                return res;
            }
        }
    }

    // 2. Domain part validation
    if (res.domainPart.length() > 253) {
        res.failureReason = "Domain part exceeds limit of 253 characters.";
        return res;
    }

    if (res.domainPart.front() == '.' || res.domainPart.back() == '.') {
        res.failureReason = "Domain part cannot begin or end with a period.";
        return res;
    }

    // Decompose domain labels
    std::istringstream domainStream(res.domainPart);
    std::string label;
    std::vector<std::string> labels;
    while (std::getline(domainStream, label, '.')) {
        if (label.empty()) {
            res.failureReason = "Domain cannot contain consecutive periods ('..').";
            return res;
        }
        if (label.length() > 63) {
            res.failureReason = "Domain label exceeds DNS limit of 63 characters: '" + label + "'.";
            return res;
        }
        if (label.front() == '-' || label.back() == '-') {
            res.failureReason = "Domain label cannot begin or end with a hyphen ('-'): '" + label + "'.";
            return res;
        }
        for (char c : label) {
            if (!isDomainChar(c)) {
                res.failureReason = std::string("Domain label contains invalid character: '") + c + "'.";
                return res;
            }
        }
        labels.push_back(label);
    }

    if (labels.size() < 2) {
        res.failureReason = "Domain must include at least one subdomain and a top-level domain (e.g. 'example.com').";
        return res;
    }

    // Top-Level Domain (TLD) must be at least 2 alpha characters
    const std::string& tld = labels.back();
    if (tld.length() < 2) {
        res.failureReason = "Top-level domain (TLD) must be at least 2 characters.";
        return res;
    }

    bool allAlpha = std::all_of(tld.begin(), tld.end(), [](unsigned char c) { return std::isalpha(c); });
    if (!allAlpha) {
        res.failureReason = "Top-level domain (TLD) must contain alphabetical characters only.";
        return res;
    }

    res.isValid = true;
    res.failureReason.clear();
    return res;
}

bool EmailValidator::isValidRegex(const std::string& email) {
    // Standard RFC 5322 simplified pattern
    static const std::regex pattern(
        R"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+$)"
    );
    return std::regex_match(email, pattern);
}

bool EmailValidator::isValidManual(const std::string& email) {
    return validate(email).isValid;
}

std::vector<ValidationResult> EmailValidator::validateBatch(const std::vector<std::string>& emails) {
    std::vector<ValidationResult> results;
    results.reserve(emails.size());
    for (const auto& e : emails) {
        results.push_back(validate(e));
    }
    return results;
}

} // namespace email_validator
