#include "TypingSpeedTest.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace typing_speed {

size_t TypingEvaluator::calculateLevenshteinDistance(const std::string& s1, const std::string& s2) {
    size_t m = s1.size();
    size_t n = s2.size();

    std::vector<std::vector<size_t>> dp(m + 1, std::vector<size_t>(n + 1, 0));

    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({
                    dp[i - 1][j],     // deletion
                    dp[i][j - 1],     // insertion
                    dp[i - 1][j - 1]  // substitution
                });
            }
        }
    }
    return dp[m][n];
}

TypingMetrics TypingEvaluator::evaluate(const std::string& reference,
                                       const std::string& typed,
                                       double elapsedSeconds) {
    TypingMetrics metrics;
    metrics.elapsedSeconds = std::max(0.001, elapsedSeconds);
    metrics.referenceLength = reference.size();
    metrics.typedLength = typed.size();

    metrics.errorChars = calculateLevenshteinDistance(reference, typed);

    // Correct characters count: characters in typed up to reference length that match exactly
    size_t minLen = std::min(reference.size(), typed.size());
    size_t directMatches = 0;
    for (size_t i = 0; i < minLen; ++i) {
        if (reference[i] == typed[i]) {
            directMatches++;
        }
    }
    metrics.correctChars = directMatches;

    // Accuracy calculation: max(0, (typedLength - errors) / typedLength)
    if (metrics.typedLength > 0) {
        if (metrics.errorChars >= metrics.typedLength) {
            metrics.accuracyPct = 0.0;
        } else {
            metrics.accuracyPct = (static_cast<double>(metrics.typedLength - metrics.errorChars) /
                                  static_cast<double>(metrics.typedLength)) * 100.0;
        }
    } else {
        metrics.accuracyPct = 0.0;
    }

    // Standard formula: 1 word = 5 characters
    double minutes = metrics.elapsedSeconds / 60.0;
    metrics.rawWpm = (static_cast<double>(metrics.typedLength) / 5.0) / minutes;

    // Net WPM = Raw WPM - (uncorrected errors / minutes)
    double penalty = (static_cast<double>(metrics.errorChars) / 5.0) / minutes;
    metrics.netWpm = std::max(0.0, metrics.rawWpm - penalty);

    if (metrics.netWpm >= 80.0) metrics.grade = "Expert Typist";
    else if (metrics.netWpm >= 60.0) metrics.grade = "Fast Professional";
    else if (metrics.netWpm >= 40.0) metrics.grade = "Average / Competent";
    else if (metrics.netWpm >= 20.0) metrics.grade = "Beginner";
    else metrics.grade = "Novice";

    return metrics;
}

std::vector<std::string> TypingEvaluator::getDefaultPrompts() {
    return {
        "The quick brown fox jumps over the lazy dog.",
        "Modern C++ emphasizes resource acquisition is initialization and standard library algorithms.",
        "Computer science is no more about computers than astronomy is about telescopes.",
        "Premature optimization is the root of all evil in software engineering."
    };
}

} // namespace typing_speed
