#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace quiz_timer {

struct Question {
    int id{0};
    std::string prompt;
    std::vector<std::string> options;
    int correctOptionIndex{0};
    int timeLimitSeconds{15};
    int basePoints{100};
};

struct QuestionResult {
    int questionId{0};
    int chosenOption{-1};
    bool isCorrect{false};
    bool timedOut{false};
    double responseTimeSeconds{0.0};
    int pointsAwarded{0};
};

struct QuizSummary {
    int totalQuestions{0};
    int correctCount{0};
    int timedOutCount{0};
    int totalScore{0};
    int maxPossibleScore{0};
    double scorePercentage{0.0};
    double averageResponseTimeSeconds{0.0};
};

class QuizTimerEngine {
public:
    QuizTimerEngine();

    void addQuestion(const Question& q);
    void loadDefaultTechnicalQuiz();

    void startQuiz();
    [[nodiscard]] bool hasNextQuestion() const;
    [[nodiscard]] const Question* getCurrentQuestion() const;

    // Answer the current question with specified response time
    QuestionResult answerCurrentQuestion(int chosenOptionIndex, double responseTimeSeconds);

    [[nodiscard]] QuizSummary calculateSummary() const;
    [[nodiscard]] const std::vector<QuestionResult>& getResults() const;
    [[nodiscard]] const std::vector<Question>& getQuestions() const;

private:
    std::vector<Question> m_questions;
    size_t m_currentIndex{0};
    std::vector<QuestionResult> m_results;
    bool m_inProgress{false};
};

} // namespace quiz_timer
