#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace palindrome_checker {

struct PalindromeResult {
    std::string originalText;
    std::string normalizedText;
    bool isPalindrome{false};
    size_t length{0};
};

// Two-pointer palindrome check on string_view (normalized or strict)
bool isPalindrome(std::string_view sv, bool ignoreCaseAndPunctuation = true);

// Strips non-alphanumeric characters and converts to lowercase
std::string normalizeString(std::string_view sv);

// Finds the longest contiguous palindromic substring within a text
std::string findLongestPalindromicSubstring(std::string_view sv);

// Batch check collection of lines/words
std::vector<PalindromeResult> checkBatch(const std::vector<std::string>& inputs, bool ignoreCaseAndPunctuation = true);

} // namespace palindrome_checker
