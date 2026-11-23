#include "PalindromeChecker.h"

#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Palindrome Word Checker CLI\n"
              << "Usage:\n"
              << "  PalindromeChecker --text \"<phrase>\"     Check single string\n"
              << "  PalindromeChecker --strict --text \"...\" Case-sensitive exact check\n"
              << "  PalindromeChecker --longest --text \"...\" Find longest palindromic substring\n"
              << "  PalindromeChecker --help                Show help\n"
              << "Examples:\n"
              << "  PalindromeChecker --text \"A man, a plan, a canal: Panama!\"\n"
              << "  PalindromeChecker --text \"racecar\"\n";
}

int main(int argc, char* argv[]) {
    std::string text;
    bool strict = false;
    bool findLongest = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--text" && i + 1 < argc) {
            text = argv[++i];
        } else if (arg == "--strict") {
            strict = true;
        } else if (arg == "--longest") {
            findLongest = true;
        }
    }

    if (!text.empty()) {
        if (findLongest) {
            std::string longest = palindrome_checker::findLongestPalindromicSubstring(text);
            std::cout << "Original text: \"" << text << "\"\n";
            std::cout << "Longest palindromic substring: \"" << longest << "\" (Length: " << longest.size() << ")\n";
            return 0;
        }

        bool result = palindrome_checker::isPalindrome(text, !strict);
        std::cout << "Text       : \"" << text << "\"\n";
        std::cout << "Mode       : " << (strict ? "Strict (exact match)" : "Normalized (ignoring punctuation & case)") << "\n";
        std::cout << "Verdict    : " << (result ? "PALINDROME (YES)" : "NOT A PALINDROME (NO)") << "\n";
        return 0;
    }

    std::cout << "=== Palindrome Word Checker ===\n";
    std::cout << "Enter a word or phrase to test: ";
    if (std::getline(std::cin, text) && !text.empty()) {
        bool result = palindrome_checker::isPalindrome(text, !strict);
        std::cout << "\nVerdict: " << (result ? "PALINDROME (YES)" : "NOT A PALINDROME (NO)") << "\n";
    }
    return 0;
}
