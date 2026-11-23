#include "TextTokenizer.hpp"
#include <cctype>
#include <algorithm>

namespace resume_ranker {

const std::unordered_set<std::string>& TextTokenizer::getStopWords() {
    static const std::unordered_set<std::string> stopWords = {
        "a", "an", "the", "and", "or", "in", "on", "at", "to", "for", "with",
        "by", "about", "against", "between", "into", "through", "during",
        "before", "after", "above", "below", "from", "up", "down", "is", "am",
        "are", "was", "were", "be", "been", "being", "have", "has", "had",
        "do", "does", "did", "but", "if", "then", "else", "when", "where",
        "why", "how", "all", "any", "both", "each", "few", "more", "most",
        "other", "some", "such", "no", "nor", "not", "only", "own", "same",
        "so", "than", "too", "very", "can", "will", "just", "should", "now"
    };
    return stopWords;
}

bool TextTokenizer::isStopWord(const std::string& word) {
    return getStopWords().count(word) > 0;
}

std::string TextTokenizer::stem(const std::string& word) {
    if (word.size() <= 4) return word;

    std::string s = word;
    // Suffix reductions: -ing, -ed, -ly, -es, -s, -tion
    if (s.size() > 5 && s.substr(s.size() - 4) == "tion") {
        s = s.substr(0, s.size() - 4);
    } else if (s.size() > 4 && s.substr(s.size() - 3) == "ing") {
        s = s.substr(0, s.size() - 3);
    } else if (s.size() > 3 && s.substr(s.size() - 2) == "ed") {
        s = s.substr(0, s.size() - 2);
    } else if (s.size() > 3 && s.substr(s.size() - 2) == "ly") {
        s = s.substr(0, s.size() - 2);
    } else if (s.size() > 3 && s.substr(s.size() - 2) == "es") {
        s = s.substr(0, s.size() - 2);
    } else if (s.back() == 's') {
        s.pop_back();
    }
    return s;
}

std::vector<std::string> TextTokenizer::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string current;

    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '+') { // preserve C++ '+'
            current += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        } else {
            if (!current.empty()) {
                if (!isStopWord(current) && current.size() > 1) {
                    tokens.push_back(stem(current));
                }
                current.clear();
            }
        }
    }
    if (!current.empty()) {
        if (!isStopWord(current) && current.size() > 1) {
            tokens.push_back(stem(current));
        }
    }

    return tokens;
}

} // namespace resume_ranker
