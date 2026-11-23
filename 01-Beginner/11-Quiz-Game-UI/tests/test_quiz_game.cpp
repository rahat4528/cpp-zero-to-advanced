#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "QuizGame.h"

using namespace quiz_game;

TEST_CASE("Question block parsing") {
    std::vector<std::string> lines = {
        "Q: What is 2 + 2?",
        "A: 3",
        "B: 4",
        "C: 5",
        "ANS: B",
        "EXP: Basic arithmetic."
    };

    auto qOpt = QuizEngine::parseQuestionBlock(lines);
    REQUIRE(qOpt.has_value());
    CHECK(qOpt->prompt == "What is 2 + 2?");
    CHECK(qOpt->options.size() == 3);
    CHECK(qOpt->correctOption == 'B');
    CHECK(qOpt->explanation == "Basic arithmetic.");
}

TEST_CASE("Scoring and percentage calculation") {
    QuizEngine engine;
    Question q1{"Q1", {"Opt1", "Opt2"}, 'A', "Exp1", "General", 10};
    Question q2{"Q2", {"Opt1", "Opt2"}, 'B', "Exp2", "General", 10};

    engine.addQuestion(q1);
    engine.addQuestion(q2);

    CHECK(engine.submitAnswer(0, 'A') == true);  // Correct
    CHECK(engine.submitAnswer(1, 'A') == false); // Incorrect (expected B)

    auto res = engine.calculateResults();
    CHECK(res.totalQuestions == 2);
    CHECK(res.correctAnswers == 1);
    CHECK(res.totalScore == 10);
    CHECK(res.maxPossibleScore == 20);
    CHECK(res.percentage == doctest::Approx(50.0));
    CHECK(res.grade.find("F") != std::string::npos);

    // Re-answer correctly
    CHECK(engine.submitAnswer(1, 'B') == true);
    auto res2 = engine.calculateResults();
    CHECK(res2.correctAnswers == 2);
    CHECK(res2.totalScore == 20);
    CHECK(res2.percentage == doctest::Approx(100.0));
    CHECK(res2.grade.find("A") != std::string::npos);
}

TEST_CASE("Reset quiz state") {
    QuizEngine engine;
    engine.addQuestion({"Q1", {"A", "B"}, 'A', "", "", 10});
    engine.submitAnswer(0, 'A');
    CHECK(engine.calculateResults().correctAnswers == 1);

    engine.reset();
    CHECK(engine.calculateResults().correctAnswers == 0);
}
