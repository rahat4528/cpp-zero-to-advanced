#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <memory>

namespace typing_tutor {

struct Lesson {
    int id{0};
    std::string title;
    std::string category; // e.g. "Home Row", "Numbers & Symbols", "C++ Keywords"
    std::string content;
    int targetWpm{30};
    double targetAccuracy{90.0};
};

struct KeystrokeResult {
    char expectedChar{'\0'};
    char typedChar{'\0'};
    bool isCorrect{false};
    double latencyMs{0.0};
};

struct TutorStats {
    int totalTyped{0};
    int correctCount{0};
    int errorCount{0};
    double elapsedTimeSeconds{0.0};
    double grossWpm{0.0};
    double netWpm{0.0};
    double accuracyPct{100.0};
    bool targetPassed{false};
    std::map<char, int> errorDistribution; // character -> count of errors
};

class TypingTutorEngine {
public:
    TypingTutorEngine();

    void loadDefaultCurriculum();
    void addLesson(const Lesson& lesson);
    [[nodiscard]] const std::vector<Lesson>& getCurriculum() const;
    [[nodiscard]] const Lesson* getLesson(int id) const;

    void startLesson(int lessonId);
    void reset();

    // Process a single typed character
    KeystrokeResult processKey(char key);

    [[nodiscard]] TutorStats calculateStats() const;
    [[nodiscard]] bool isCompleted() const;
    [[nodiscard]] size_t getCurrentIndex() const;
    [[nodiscard]] const Lesson* getCurrentLesson() const;
    [[nodiscard]] const std::vector<KeystrokeResult>& getKeystrokes() const;

private:
    std::vector<Lesson> m_curriculum;
    const Lesson* m_currentLesson{nullptr};
    size_t m_cursorIndex{0};
    std::chrono::steady_clock::time_point m_startTime;
    std::chrono::steady_clock::time_point m_lastKeystrokeTime;
    bool m_sessionActive{false};
    std::vector<KeystrokeResult> m_keystrokes;
    std::map<char, int> m_errorCounts;
};

} // namespace typing_tutor
