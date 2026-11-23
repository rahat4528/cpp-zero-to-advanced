#pragma once

#include <string>
#include <vector>
#include <optional>

namespace quiz_game {

struct Question {
    std::string prompt;
    std::vector<std::string> options; // typically 4 options
    char correctOption{'A'};          // 'A', 'B', 'C', or 'D'
    std::string explanation;
    std::string category;
    int points{10};
};

struct QuizResult {
    size_t totalQuestions{0};
    size_t correctAnswers{0};
    int totalScore{0};
    int maxPossibleScore{0};
    double percentage{0.0};
    std::string grade;
};

class QuizEngine {
public:
    QuizEngine() = default;

    bool loadQuestionsFromFile(const std::string& filepath);
    void addQuestion(const Question& q);

    size_t questionCount() const { return m_questions.size(); }
    const Question& getQuestion(size_t index) const;

    // Submits an answer for the given question index, returns true if correct
    bool submitAnswer(size_t index, char answer);

    // Resets quiz state for a new playthrough
    void reset();

    // Final results
    QuizResult calculateResults() const;

    static std::optional<Question> parseQuestionBlock(const std::vector<std::string>& lines);

private:
    std::vector<Question> m_questions;
    std::vector<bool> m_answeredCorrectly;
};

} // namespace quiz_game
