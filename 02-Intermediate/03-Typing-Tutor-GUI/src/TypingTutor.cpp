#include "TypingTutor.hpp"
#include <stdexcept>
#include <algorithm>

namespace typing_tutor {

TypingTutorEngine::TypingTutorEngine() {
    loadDefaultCurriculum();
}

void TypingTutorEngine::loadDefaultCurriculum() {
    m_curriculum = {
        {
            1,
            "Home Row Basics",
            "Fundamentals",
            "asdf jkl; asdf jkl; aadd ssff jjkk ll;;",
            25,
            90.0
        },
        {
            2,
            "Common English Bigrams",
            "Intermediate",
            "the that with this from they were would there about",
            35,
            92.0
        },
        {
            3,
            "Punctuation & Numeric Symbols",
            "Intermediate",
            "123 + 456 = 579; if (x >= 10 && y <= 20) return true;",
            30,
            90.0
        },
        {
            4,
            "Modern C++ Idioms",
            "Advanced",
            "template <typename T> constexpr auto square(T x) noexcept -> decltype(x * x) { return x * x; }",
            40,
            95.0
        }
    };
}

void TypingTutorEngine::addLesson(const Lesson& lesson) {
    m_curriculum.push_back(lesson);
}

const std::vector<Lesson>& TypingTutorEngine::getCurriculum() const {
    return m_curriculum;
}

const Lesson* TypingTutorEngine::getLesson(int id) const {
    for (const auto& l : m_curriculum) {
        if (l.id == id) {
            return &l;
        }
    }
    return nullptr;
}

void TypingTutorEngine::startLesson(int lessonId) {
    const auto* lesson = getLesson(lessonId);
    if (!lesson) {
        throw std::invalid_argument("Invalid lesson ID: " + std::to_string(lessonId));
    }
    m_currentLesson = lesson;
    reset();
}

void TypingTutorEngine::reset() {
    m_cursorIndex = 0;
    m_sessionActive = false;
    m_keystrokes.clear();
    m_errorCounts.clear();
}

KeystrokeResult TypingTutorEngine::processKey(char key) {
    if (!m_currentLesson) {
        throw std::runtime_error("No lesson currently active");
    }

    if (m_cursorIndex >= m_currentLesson->content.length()) {
        return {'\0', key, false, 0.0};
    }

    auto now = std::chrono::steady_clock::now();
    if (!m_sessionActive) {
        m_sessionActive = true;
        m_startTime = now;
        m_lastKeystrokeTime = now;
    }

    double deltaMs = std::chrono::duration<double, std::milli>(now - m_lastKeystrokeTime).count();
    m_lastKeystrokeTime = now;

    char expected = m_currentLesson->content[m_cursorIndex];
    bool correct = (key == expected);

    if (!correct) {
        m_errorCounts[expected]++;
    }

    KeystrokeResult res{expected, key, correct, deltaMs};
    m_keystrokes.push_back(res);

    m_cursorIndex++;
    return res;
}

TutorStats TypingTutorEngine::calculateStats() const {
    TutorStats stats;
    stats.totalTyped = static_cast<int>(m_keystrokes.size());
    stats.errorDistribution = m_errorCounts;

    if (stats.totalTyped == 0) {
        return stats;
    }

    for (const auto& k : m_keystrokes) {
        if (k.isCorrect) {
            stats.correctCount++;
        } else {
            stats.errorCount++;
        }
    }

    if (m_sessionActive) {
        stats.elapsedTimeSeconds = std::chrono::duration<double>(
            m_lastKeystrokeTime - m_startTime
        ).count();
    }

    if (stats.elapsedTimeSeconds < 0.001) {
        stats.elapsedTimeSeconds = 0.001; // Avoid div by zero
    }

    double minutes = stats.elapsedTimeSeconds / 60.0;
    // Standard formula: 1 word = 5 characters
    stats.grossWpm = (static_cast<double>(stats.totalTyped) / 5.0) / minutes;
    double netWords = (static_cast<double>(stats.totalTyped) / 5.0) - (static_cast<double>(stats.errorCount) / 5.0);
    stats.netWpm = std::max(0.0, netWords / minutes);
    stats.accuracyPct = (static_cast<double>(stats.correctCount) / static_cast<double>(stats.totalTyped)) * 100.0;

    if (m_currentLesson) {
        stats.targetPassed = (stats.netWpm >= m_currentLesson->targetWpm) &&
                             (stats.accuracyPct >= m_currentLesson->targetAccuracy) &&
                             isCompleted();
    }

    return stats;
}

bool TypingTutorEngine::isCompleted() const {
    if (!m_currentLesson) return false;
    return m_cursorIndex >= m_currentLesson->content.length();
}

size_t TypingTutorEngine::getCurrentIndex() const {
    return m_cursorIndex;
}

const Lesson* TypingTutorEngine::getCurrentLesson() const {
    return m_currentLesson;
}

const std::vector<KeystrokeResult>& TypingTutorEngine::getKeystrokes() const {
    return m_keystrokes;
}

} // namespace typing_tutor
