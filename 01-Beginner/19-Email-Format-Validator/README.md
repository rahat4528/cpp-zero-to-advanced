# 19-Email-Format-Validator

An RFC 5322 and RFC 5321 compliant Email Address Syntax and Domain Validator with comprehensive error diagnostics built with modern C++17.

---

## 1. Project Overview
Email Format Validator parses and validates email addresses against international standards (RFC 5321 / RFC 5322). It provides dual validation engines: a regular expression engine using `<regex>` and a high-performance manual finite-state machine with granular diagnostic failure reasons.

## 2. Problem Statement
Naïve email checks (e.g. simply checking for an `@` symbol) allow syntactically malformed addresses to pollute databases, causing email delivery failure. Proper verification requires checking character sets, consecutive period rules, label lengths, and top-level domain constraints.

## 3. Objectives
- Implement comprehensive RFC 5321 / 5322 specification checks.
- Enforce strict size boundaries (max 64 chars local-part, max 253 chars domain, max 254 chars total).
- Validate domain label DNS rules (no leading/trailing hyphens, no consecutive dots, alpha TLD >= 2 chars).
- Provide human-readable diagnostic error explanations for invalid entries.

## 4. Features
- **Dual Engine**: Features both standard `<regex>` and manual zero-overhead tokenizer.
- **Detailed Diagnostics**: Pinpoints the exact violation (e.g. "Domain label cannot begin with hyphen").
- **Length Protections**: Protects against buffer denial-of-service attempts.
- **CLI Automation**: Instant scriptable validation via `--email <address>`.

## 5. Technology Stack
- **Language**: C++17 (`<regex>`, `std::vector`, `std::string`, `std::istringstream`, `<cctype>`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No. Pure syntax and DNS label grammar validation.

## 6. Folder Structure
```
19-Email-Format-Validator/
├── include/
│   └── EmailValidator.h         # ValidationResult and EmailValidator API
├── src/
│   ├── EmailValidator.cpp       # Regex & manual state tokenizer logic
│   └── main.cpp                 # CLI driver and report formatter
├── tests/
│   └── test_email_validator.cpp # doctest unit test suite
├── CMakeLists.txt               # CMake build configuration
├── .gitignore                   # Build ignore rules
└── README.md                    # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Validate valid email
.\build\EmailValidator.exe --email "user.name+tag@sub.example.com"

# Validate invalid email
.\build\EmailValidator.exe --email "invalid..dots@domain.org"
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/EmailValidator --email "user@domain.com"
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/EmailValidator --email "user.name+tag@sub.example.com"
========================================================
               EMAIL VALIDATION REPORT                  
========================================================
Input Address : user.name+tag@sub.example.com
Validation    : VALID [PASS]
Local Part    : user.name+tag (Length: 13)
Domain Part   : sub.example.com (Length: 15)
========================================================
```

## 11. Known Limitations
- Validates syntax and DNS formatting; does not perform active network MX record lookups (intentionally offline for beginner tier).
- Internationalized Domain Names (IDN/Punycode) must be passed in ASCII A-label format (`xn--...`).

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, tested both valid and invalid email addresses.
- **Tested**: Yes, 4 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
