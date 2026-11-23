#pragma once

#include <string>
#include <vector>

namespace email_validator {

struct ValidationResult {
    std::string email;
    bool isValid{false};
    std::string localPart;
    std::string domainPart;
    std::string failureReason; // Detailed diagnostic code / explanation
};

class EmailValidator {
public:
    // Full RFC 5322 syntax validation with diagnostic feedback
    static ValidationResult validate(const std::string& email);

    // Fast boolean check using standard regex
    static bool isValidRegex(const std::string& email);

    // Fast boolean check using manual state-machine parsing (zero regex overhead)
    static bool isValidManual(const std::string& email);

    // Batch validation helper
    static std::vector<ValidationResult> validateBatch(const std::vector<std::string>& emails);
};

} // namespace email_validator
