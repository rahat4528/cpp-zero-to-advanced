#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "TypingSpeedTest.h"

using namespace typing_speed;

TEST_CASE("Levenshtein edit distance calculations") {
    CHECK(TypingEvaluator::calculateLevenshteinDistance("kitten", "sitting") == 3);
    CHECK(TypingEvaluator::calculateLevenshteinDistance("hello", "hello") == 0);
    CHECK(TypingEvaluator::calculateLevenshteinDistance("", "abc") == 3);
    CHECK(TypingEvaluator::calculateLevenshteinDistance("abc", "") == 3);
    CHECK(TypingEvaluator::calculateLevenshteinDistance("test", "tent") == 1);
}

TEST_CASE("Perfect typing WPM calculation") {
    // 50 characters in 60 seconds = 10 words / 1 minute = 10 WPM
    std::string ref = "12345678901234567890123456789012345678901234567890"; // 50 chars
    auto m = TypingEvaluator::evaluate(ref, ref, 60.0);

    CHECK(m.typedLength == 50);
    CHECK(m.errorChars == 0);
    CHECK(m.accuracyPct == doctest::Approx(100.0));
    CHECK(m.rawWpm == doctest::Approx(10.0));
    CHECK(m.netWpm == doctest::Approx(10.0));
}

TEST_CASE("Typing with typos reduces net WPM") {
    std::string ref = "The quick brown fox jumps"; // 25 chars
    std::string typed = "The quick brown fox jumpz"; // 1 substitution error (s -> z)

    // Completed in 15 seconds (0.25 minutes)
    // Raw WPM = (25 / 5) / 0.25 = 20 WPM
    // Error penalty = (1 error / 5 chars) / 0.25 = 0.8 WPM
    // Net WPM = 20 - 0.8 = 19.2 WPM
    auto m = TypingEvaluator::evaluate(ref, typed, 15.0);

    CHECK(m.errorChars == 1);
    CHECK(m.rawWpm == doctest::Approx(20.0));
    CHECK(m.netWpm == doctest::Approx(19.2));
    CHECK(m.accuracyPct == doctest::Approx(96.0));
}

TEST_CASE("Zero or negative time boundary") {
    auto m = TypingEvaluator::evaluate("hello", "hello", 0.0);
    CHECK(m.elapsedSeconds > 0.0); // Guarded against division by zero
    CHECK(m.netWpm > 0.0);
}
