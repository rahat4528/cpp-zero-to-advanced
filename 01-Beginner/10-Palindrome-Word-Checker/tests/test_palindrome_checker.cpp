#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "PalindromeChecker.h"

using namespace palindrome_checker;

TEST_CASE("Basic word palindromes") {
    CHECK(isPalindrome("racecar") == true);
    CHECK(isPalindrome("level") == true);
    CHECK(isPalindrome("radar") == true);
    CHECK(isPalindrome("hello") == false);
    CHECK(isPalindrome("world") == false);
}

TEST_CASE("Empty and single character edge cases") {
    CHECK(isPalindrome("") == true);
    CHECK(isPalindrome("a") == true);
    CHECK(isPalindrome("Z") == true);
}

TEST_CASE("Phrase palindromes with punctuation and mixed casing") {
    CHECK(isPalindrome("A man, a plan, a canal: Panama!") == true);
    CHECK(isPalindrome("Was it a car or a cat I saw?") == true);
    CHECK(isPalindrome("No 'x' in Nixon") == true);
    CHECK(isPalindrome("Madam, I'm Adam") == true);
    CHECK(isPalindrome("Not a palindrome at all") == false);
}

TEST_CASE("Strict mode respects casing and punctuation") {
    CHECK(isPalindrome("Racecar", false) == false); // Casing mismatch
    CHECK(isPalindrome("racecar", false) == true);
    CHECK(isPalindrome("a man, a plan", false) == false);
}

TEST_CASE("String normalization") {
    CHECK(normalizeString("A man, a plan!") == "amanaplan");
    CHECK(normalizeString("123-abc-321") == "123abc321");
    CHECK(normalizeString("...").empty());
}

TEST_CASE("Longest palindromic substring") {
    CHECK(findLongestPalindromicSubstring("babad") == "bab"); // or "aba"
    CHECK(findLongestPalindromicSubstring("cbbd") == "bb");
    CHECK(findLongestPalindromicSubstring("racecar") == "racecar");
    CHECK(findLongestPalindromicSubstring("abcdef") == "a");
}
