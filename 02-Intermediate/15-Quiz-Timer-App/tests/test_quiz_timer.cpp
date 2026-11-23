#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "QuizTimerEngine.hpp"

TEST_CASE("QuizTimerEngine - Question Progression and Answers") {
    quiz_timer::QuizTimerEngine engine;
    CHECK(engine.getQuestions().size() == 4);

    engine.startQuiz();
    REQUIRE(engine.hasNextQuestion() == true);

    const auto* q1 = engine.getCurrentQuestion();
    REQUIRE(q1 != nullptr);
    CHECK(q1->id == 1);

    // Fast correct answer: 1.0 second on a 15 second question
    auto res1 = engine.answerCurrentQuestion(1, 1.0);
    CHECK(res1.isCorrect == true);
    CHECK(res1.timedOut == false);
    CHECK(res1.pointsAwarded > 100); // Has speed bonus

    // Timeout answer: 20 seconds on 10s question
    auto res2 = engine.answerCurrentQuestion(0, 20.0);
    CHECK(res2.isCorrect == false);
    CHECK(res2.timedOut == true);
    CHECK(res2.pointsAwarded == 0);

    // Wrong answer
    auto res3 = engine.answerCurrentQuestion(0, 2.0);
    CHECK(res3.isCorrect == false);
    CHECK(res3.timedOut == false);
    CHECK(res3.pointsAwarded == 0);

    // Normal correct answer
    auto res4 = engine.answerCurrentQuestion(2, 5.0);
    CHECK(res4.isCorrect == true);
    CHECK(res4.pointsAwarded >= 100);

    CHECK(engine.hasNextQuestion() == false);

    auto summary = engine.calculateSummary();
    CHECK(summary.totalQuestions == 4);
    CHECK(summary.correctCount == 2);
    CHECK(summary.timedOutCount == 1);
    CHECK(summary.totalScore > 0);
}

TEST_CASE("QuizTimerEngine - Custom Question Bank") {
    quiz_timer::QuizTimerEngine engine;
    quiz_timer::Question customQ{
        99,
        "What is the default language standard for this repository?",
        {"C++11", "C++14", "C++17", "C++20"},
        2, // C++17
        5,
        200
    };
    engine.addQuestion(customQ);
    CHECK(engine.getQuestions().size() == 5);
}
