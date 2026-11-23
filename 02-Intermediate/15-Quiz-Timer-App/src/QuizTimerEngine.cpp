#include "QuizTimerEngine.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace quiz_timer {

QuizTimerEngine::QuizTimerEngine() {
    loadDefaultTechnicalQuiz();
}

void QuizTimerEngine::addQuestion(const Question& q) {
    m_questions.push_back(q);
}

void QuizTimerEngine::loadDefaultTechnicalQuiz() {
    m_questions = {
        {
            1,
            "Which C++ keyword prevents a virtual function from being overridden in derived classes?",
            {"override", "final", "const", "sealed"},
            1, // final
            15,
            100
        },
        {
            2,
            "What is the time complexity of looking up a key in std::unordered_map on average?",
            {"O(1)", "O(log N)", "O(N)", "O(N log N)"},
            0, // O(1)
            10,
            100
        },
        {
            3,
            "Which smart pointer allows shared ownership with reference counting?",
            {"std::unique_ptr", "std::shared_ptr", "std::weak_ptr", "std::auto_ptr"},
            1, // std::shared_ptr
            12,
            100
        },
        {
            4,
            "What idiom ensures resources are released when stack objects go out of scope?",
            {"SFINAE", "CRTP", "RAII", "PIMPL"},
            2, // RAII
            10,
            100
        }
    };
}

void QuizTimerEngine::startQuiz() {
    m_currentIndex = 0;
    m_results.clear();
    m_inProgress = !m_questions.empty();
}

bool QuizTimerEngine::hasNextQuestion() const {
    return m_inProgress && (m_currentIndex < m_questions.size());
}

const Question* QuizTimerEngine::getCurrentQuestion() const {
    if (!hasNextQuestion()) return nullptr;
    return &m_questions[m_currentIndex];
}

QuestionResult QuizTimerEngine::answerCurrentQuestion(int chosenOptionIndex, double responseTimeSeconds) {
    if (!hasNextQuestion()) {
        return {};
    }

    const auto& q = m_questions[m_currentIndex];
    QuestionResult res;
    res.questionId = q.id;
    res.chosenOption = chosenOptionIndex;
    res.responseTimeSeconds = responseTimeSeconds;

    if (responseTimeSeconds > static_cast<double>(q.timeLimitSeconds)) {
        res.timedOut = true;
        res.isCorrect = false;
        res.pointsAwarded = 0;
    } else {
        res.timedOut = false;
        res.isCorrect = (chosenOptionIndex == q.correctOptionIndex);

        if (res.isCorrect) {
            // Speed bonus: full base points + up to 50% extra for instantaneous answer
            double speedFactor = 1.0 - (responseTimeSeconds / static_cast<double>(q.timeLimitSeconds));
            speedFactor = std::clamp(speedFactor, 0.0, 1.0);
            int bonus = static_cast<int>(std::round(q.basePoints * 0.5 * speedFactor));
            res.pointsAwarded = q.basePoints + bonus;
        } else {
            res.pointsAwarded = 0;
        }
    }

    m_results.push_back(res);
    m_currentIndex++;
    if (m_currentIndex >= m_questions.size()) {
        m_inProgress = false;
    }
    return res;
}

QuizSummary QuizTimerEngine::calculateSummary() const {
    QuizSummary summary;
    summary.totalQuestions = static_cast<int>(m_questions.size());

    double totalTime = 0.0;
    for (const auto& r : m_results) {
        if (r.isCorrect) summary.correctCount++;
        if (r.timedOut) summary.timedOutCount++;
        summary.totalScore += r.pointsAwarded;
        totalTime += r.responseTimeSeconds;
    }

    for (const auto& q : m_questions) {
        // Max score = base + 50% max speed bonus
        summary.maxPossibleScore += q.basePoints + static_cast<int>(q.basePoints * 0.5);
    }

    if (summary.maxPossibleScore > 0) {
        summary.scorePercentage = (static_cast<double>(summary.totalScore) / static_cast<double>(summary.maxPossibleScore)) * 100.0;
    }

    if (!m_results.empty()) {
        summary.averageResponseTimeSeconds = totalTime / static_cast<double>(m_results.size());
    }

    return summary;
}

const std::vector<QuestionResult>& QuizTimerEngine::getResults() const {
    return m_results;
}

const std::vector<Question>& QuizTimerEngine::getQuestions() const {
    return m_questions;
}

} // namespace quiz_timer
