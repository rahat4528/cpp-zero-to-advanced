#include "PalindromeChecker.h"

#include <algorithm>
#include <cctype>

namespace palindrome_checker {

std::string normalizeString(std::string_view sv) {
    std::string normalized;
    normalized.reserve(sv.size());
    for (char c : sv) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
    }
    return normalized;
}

bool isPalindrome(std::string_view sv, bool ignoreCaseAndPunctuation) {
    if (sv.empty()) return true;

    if (!ignoreCaseAndPunctuation) {
        if (sv.size() <= 1) return true;
        size_t left = 0;
        size_t right = sv.size() - 1;
        while (left < right) {
            if (sv[left] != sv[right]) return false;
            left++;
            right--;
        }
        return true;
    }

    // Normalized two-pointer check without allocating a new string
    if (sv.size() <= 1) return true;
    int left = 0;
    int right = static_cast<int>(sv.size()) - 1;

    while (left < right) {
        while (left < right && !std::isalnum(static_cast<unsigned char>(sv[left]))) {
            left++;
        }
        while (left < right && !std::isalnum(static_cast<unsigned char>(sv[right]))) {
            right--;
        }
        if (left < right) {
            char cl = static_cast<char>(std::tolower(static_cast<unsigned char>(sv[left])));
            char cr = static_cast<char>(std::tolower(static_cast<unsigned char>(sv[right])));
            if (cl != cr) {
                return false;
            }
            left++;
            right--;
        }
    }
    return true;
}

std::string findLongestPalindromicSubstring(std::string_view sv) {
    if (sv.empty()) return "";
    size_t n = sv.size();
    size_t start = 0;
    size_t maxLen = 1;

    auto expandAroundCenter = [&](int left, int right) {
        while (left >= 0 && right < static_cast<int>(n) && sv[left] == sv[right]) {
            left--;
            right++;
        }
        return right - left - 1;
    };

    for (size_t i = 0; i < n; ++i) {
        int len1 = expandAroundCenter(static_cast<int>(i), static_cast<int>(i));
        int len2 = expandAroundCenter(static_cast<int>(i), static_cast<int>(i + 1));
        int len = std::max(len1, len2);
        if (static_cast<size_t>(len) > maxLen) {
            maxLen = static_cast<size_t>(len);
            start = i - (len - 1) / 2;
        }
    }

    return std::string(sv.substr(start, maxLen));
}

std::vector<PalindromeResult> checkBatch(const std::vector<std::string>& inputs, bool ignoreCaseAndPunctuation) {
    std::vector<PalindromeResult> results;
    results.reserve(inputs.size());
    for (const auto& item : inputs) {
        PalindromeResult r;
        r.originalText = item;
        r.normalizedText = normalizeString(item);
        r.isPalindrome = isPalindrome(item, ignoreCaseAndPunctuation);
        r.length = ignoreCaseAndPunctuation ? r.normalizedText.size() : item.size();
        results.push_back(r);
    }
    return results;
}

} // namespace palindrome_checker
