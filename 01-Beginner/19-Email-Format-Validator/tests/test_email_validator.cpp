#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "EmailValidator.h"

using namespace email_validator;

TEST_CASE("Valid RFC 5322 email patterns") {
    std::vector<std::string> validEmails = {
        "user@example.com",
        "first.last@sub.domain.org",
        "customer+tag@company.co.uk",
        "123456@numbers.com",
        "disposable.style.email.with+symbol@example.com",
        "other.email-with-dash@example.com"
    };

    for (const auto& e : validEmails) {
        auto res = EmailValidator::validate(e);
        CHECK(res.isValid == true);
        CHECK(EmailValidator::isValidRegex(e) == true);
    }
}

TEST_CASE("Local part invalid boundaries") {
    // Cannot start with dot
    auto r1 = EmailValidator::validate(".user@example.com");
    CHECK(r1.isValid == false);
    CHECK(r1.failureReason.find("period") != std::string::npos);

    // Cannot end with dot
    auto r2 = EmailValidator::validate("user.@example.com");
    CHECK(r2.isValid == false);

    // Cannot contain consecutive dots
    auto r3 = EmailValidator::validate("john..doe@example.com");
    CHECK(r3.isValid == false);
    CHECK(r3.failureReason.find("consecutive") != std::string::npos);

    // Missing local part
    auto r4 = EmailValidator::validate("@example.com");
    CHECK(r4.isValid == false);
}

TEST_CASE("Domain part invalid boundaries") {
    // Missing domain
    CHECK(EmailValidator::validate("user@").isValid == false);

    // Domain label cannot start with hyphen
    auto r1 = EmailValidator::validate("user@-domain.com");
    CHECK(r1.isValid == false);
    CHECK(r1.failureReason.find("hyphen") != std::string::npos);

    // Domain label cannot end with hyphen
    auto r2 = EmailValidator::validate("user@domain-.com");
    CHECK(r2.isValid == false);

    // Domain cannot have consecutive dots
    auto r3 = EmailValidator::validate("user@domain..com");
    CHECK(r3.isValid == false);

    // Missing TLD
    auto r4 = EmailValidator::validate("user@localhost");
    CHECK(r4.isValid == false);

    // Numeric TLD
    auto r5 = EmailValidator::validate("user@domain.123");
    CHECK(r5.isValid == false);
}

TEST_CASE("Length limits") {
    // Local part exceeds 64 chars
    std::string longLocal(65, 'a');
    longLocal += "@example.com";
    auto r1 = EmailValidator::validate(longLocal);
    CHECK(r1.isValid == false);
    CHECK(r1.failureReason.find("64") != std::string::npos);

    // Total length exceeds 254 chars
    std::string hugeEmail = "user@" + std::string(250, 'a') + ".com";
    auto r2 = EmailValidator::validate(hugeEmail);
    CHECK(r2.isValid == false);
    CHECK(r2.failureReason.find("254") != std::string::npos);
}
