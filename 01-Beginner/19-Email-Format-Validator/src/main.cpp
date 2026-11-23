#include "EmailValidator.h"

#include <iostream>
#include <string>

void printHelp() {
    std::cout << "RFC 5322 Email Format Validator CLI\n"
              << "Usage:\n"
              << "  EmailValidator --email <address>             Validate single email address\n"
              << "  EmailValidator --help                        Show help\n"
              << "Examples:\n"
              << "  EmailValidator --email \"user.name@example.com\"\n"
              << "  EmailValidator --email \"invalid..dots@domain.org\"\n";
}

int main(int argc, char* argv[]) {
    std::string email;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--email" && i + 1 < argc) {
            email = argv[++i];
        }
    }

    if (!email.empty()) {
        auto res = email_validator::EmailValidator::validate(email);
        std::cout << "========================================================\n"
                  << "               EMAIL VALIDATION REPORT                  \n"
                  << "========================================================\n"
                  << "Input Address : " << res.email << "\n"
                  << "Validation    : " << (res.isValid ? "VALID [PASS]" : "INVALID [FAIL]") << "\n";
        if (res.isValid) {
            std::cout << "Local Part    : " << res.localPart << " (Length: " << res.localPart.size() << ")\n"
                      << "Domain Part   : " << res.domainPart << " (Length: " << res.domainPart.size() << ")\n";
        } else {
            std::cout << "Reason/Error  : " << res.failureReason << "\n";
        }
        std::cout << "========================================================\n";
        return res.isValid ? 0 : 1;
    }

    std::cout << "=== Email Format Validator ===\n";
    std::cout << "Enter email address to validate: ";
    if (std::cin >> email) {
        auto res = email_validator::EmailValidator::validate(email);
        std::cout << "Result: " << (res.isValid ? "VALID" : "INVALID") << "\n";
        if (!res.isValid) {
            std::cout << "Reason: " << res.failureReason << "\n";
        }
        return res.isValid ? 0 : 1;
    }

    return 0;
}
