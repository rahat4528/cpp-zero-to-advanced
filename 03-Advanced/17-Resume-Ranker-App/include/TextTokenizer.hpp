#pragma once

#include <string>
#include <vector>
#include <unordered_set>

namespace resume_ranker {

class TextTokenizer {
public:
    [[nodiscard]] static std::vector<std::string> tokenize(const std::string& text);
    [[nodiscard]] static std::string stem(const std::string& word);
    [[nodiscard]] static bool isStopWord(const std::string& word);

private:
    [[nodiscard]] static const std::unordered_set<std::string>& getStopWords();
};

} // namespace resume_ranker
