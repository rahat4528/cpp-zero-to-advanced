#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace typing_speed {

struct TypingMetrics {
    double elapsedSeconds{0.0};
    size_t referenceLength{0};
    size_t typedLength{0};
    size_t correctChars{0};
    size_t errorChars{0}; // Levenshtein edit distance
    double accuracyPct{0.0};
    double rawWpm{0.0};
    double netWpm{0.0};
    std::string grade;
};

class TypingEvaluator {
public:
    // Calculates edit distance (insertions, deletions, substitutions)
    static size_t calculateLevenshteinDistance(const std::string& target, const std::string& typed);

    // Evaluates typing performance against reference text and elapsed time
    static TypingMetrics evaluate(const std::string& reference,
                                 const std::string& typed,
                                 double elapsedSeconds);

    // Default library of prompt texts
    static std::vector<std::string> getDefaultPrompts();
};

} // namespace typing_speed
