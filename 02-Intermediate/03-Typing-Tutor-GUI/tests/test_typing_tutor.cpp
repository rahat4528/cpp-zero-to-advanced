#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "TypingTutor.hpp"

TEST_CASE("TypingTutorEngine - Curriculum Initialization") {
    typing_tutor::TypingTutorEngine engine;
    const auto& curriculum = engine.getCurriculum();
    CHECK(curriculum.size() >= 4);

    const auto* l1 = engine.getLesson(1);
    REQUIRE(l1 != nullptr);
    CHECK(l1->title == "Home Row Basics");
    CHECK(l1->targetWpm == 25);
    CHECK(l1->targetAccuracy == 90.0);

    CHECK(engine.getLesson(999) == nullptr);
}

TEST_CASE("TypingTutorEngine - Perfect Typing Execution") {
    typing_tutor::TypingTutorEngine engine;
    engine.startLesson(1);
    const auto* l1 = engine.getCurrentLesson();
    REQUIRE(l1 != nullptr);

    for (char c : l1->content) {
        auto res = engine.processKey(c);
        CHECK(res.isCorrect == true);
    }

    CHECK(engine.isCompleted() == true);
    auto stats = engine.calculateStats();
    CHECK(stats.totalTyped == static_cast<int>(l1->content.length()));
    CHECK(stats.correctCount == stats.totalTyped);
    CHECK(stats.errorCount == 0);
    CHECK(stats.accuracyPct == 100.0);
    CHECK(stats.grossWpm > 0.0);
}

TEST_CASE("TypingTutorEngine - Error Handling & Mistakes Distribution") {
    typing_tutor::TypingTutorEngine engine;
    engine.startLesson(1);
    const auto* l1 = engine.getCurrentLesson();
    REQUIRE(l1 != nullptr);

    // Type 4 characters, 2 wrong
    engine.processKey(l1->content[0]); // correct
    engine.processKey('z');            // wrong, expected l1->content[1]
    engine.processKey(l1->content[2]); // correct
    engine.processKey('x');            // wrong, expected l1->content[3]

    auto stats = engine.calculateStats();
    CHECK(stats.totalTyped == 4);
    CHECK(stats.correctCount == 2);
    CHECK(stats.errorCount == 2);
    CHECK(stats.accuracyPct == 50.0);
    CHECK(stats.errorDistribution[l1->content[1]] == 1);
    CHECK(stats.errorDistribution[l1->content[3]] == 1);
    CHECK(engine.isCompleted() == false);
}

TEST_CASE("TypingTutorEngine - Custom Lesson Insertion") {
    typing_tutor::TypingTutorEngine engine;
    typing_tutor::Lesson customLesson{
        100,
        "Custom Regex Practice",
        "Regex",
        "^([a-z]+)@([a-z]+)\\.com$",
        45,
        98.0
    };
    engine.addLesson(customLesson);
    const auto* retrieved = engine.getLesson(100);
    REQUIRE(retrieved != nullptr);
    CHECK(retrieved->title == "Custom Regex Practice");
}
