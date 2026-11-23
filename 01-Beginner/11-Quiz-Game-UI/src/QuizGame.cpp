#include "QuizGame.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace quiz_game {

bool QuizEngine::loadQuestionsFromFile(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) return false;

    std::vector<std::string> block;
    std::string line;

    while (std::getline(in, line)) {
        // Strip trailing CR if Windows line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty() || line == "---") {
            if (!block.empty()) {
                auto qOpt = parseQuestionBlock(block);
                if (qOpt) addQuestion(*qOpt);
                block.clear();
            }
        } else if (line.front() != '#') {
            block.push_back(line);
        }
    }

    if (!block.empty()) {
        auto qOpt = parseQuestionBlock(block);
        if (qOpt) addQuestion(*qOpt);
    }

    return !m_questions.empty();
}

std::optional<Question> QuizEngine::parseQuestionBlock(const std::vector<std::string>& lines) {
    // Expected format:
    // Q: Question text
    // A: Option A
    // B: Option B
    // C: Option C
    // D: Option D
    // ANS: A
    // EXP: Explanation text
    Question q;
    for (const auto& line : lines) {
        if (line.rfind("Q: ", 0) == 0) {
            q.prompt = line.substr(3);
        } else if (line.rfind("A: ", 0) == 0) {
            q.options.push_back(line.substr(3));
        } else if (line.rfind("B: ", 0) == 0) {
            q.options.push_back(line.substr(3));
        } else if (line.rfind("C: ", 0) == 0) {
            q.options.push_back(line.substr(3));
        } else if (line.rfind("D: ", 0) == 0) {
            q.options.push_back(line.substr(3));
        } else if (line.rfind("ANS: ", 0) == 0) {
            std::string ans = line.substr(5);
            if (!ans.empty()) {
                q.correctOption = static_cast<char>(std::toupper(static_cast<unsigned char>(ans.front())));
            }
        } else if (line.rfind("EXP: ", 0) == 0) {
            q.explanation = line.substr(5);
        }
    }

    if (q.prompt.empty() || q.options.size() < 2) {
        return std::nullopt;
    }
    return q;
}

void QuizEngine::addQuestion(const Question& q) {
    m_questions.push_back(q);
    m_answeredCorrectly.push_back(false);
}

const Question& QuizEngine::getQuestion(size_t index) const {
    return m_questions.at(index);
}

bool QuizEngine::submitAnswer(size_t index, char answer) {
    if (index >= m_questions.size()) return false;
    char normalized = static_cast<char>(std::toupper(static_cast<unsigned char>(answer)));
    bool isCorrect = (normalized == m_questions[index].correctOption);
    m_answeredCorrectly[index] = isCorrect;
    return isCorrect;
}

void QuizEngine::reset() {
    std::fill(m_answeredCorrectly.begin(), m_answeredCorrectly.end(), false);
}

QuizResult QuizEngine::calculateResults() const {
    QuizResult res;
    res.totalQuestions = m_questions.size();
    if (res.totalQuestions == 0) return res;

    for (size_t i = 0; i < m_questions.size(); ++i) {
        res.maxPossibleScore += m_questions[i].points;
        if (m_answeredCorrectly[i]) {
            res.correctAnswers++;
            res.totalScore += m_questions[i].points;
        }
    }

    res.percentage = (static_cast<double>(res.totalScore) / static_cast<double>(res.maxPossibleScore)) * 100.0;
    if (res.percentage >= 90.0) res.grade = "A (Outstanding)";
    else if (res.percentage >= 80.0) res.grade = "B (Good)";
    else if (res.percentage >= 70.0) res.grade = "C (Satisfactory)";
    else if (res.percentage >= 60.0) res.grade = "D (Passing)";
    else res.grade = "F (Needs Improvement)";

    return res;
}

} // namespace quiz_game
