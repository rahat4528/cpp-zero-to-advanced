#pragma once

#include <string>
#include <vector>

namespace editor {

enum class TokenType {
    Keyword,
    PrimitiveType,
    StringLiteral,
    CharLiteral,
    NumberLiteral,
    Comment,
    Preprocessor,
    Identifier,
    Punctuation,
    Whitespace
};

struct SyntaxToken {
    TokenType type;
    std::string text;
};

class CppSyntaxHighlighter {
public:
    static std::vector<SyntaxToken> tokenizeLine(const std::string& line);
    static std::string renderAnsiColoredLine(const std::string& line);

private:
    static bool isKeyword(const std::string& word);
    static bool isPrimitiveType(const std::string& word);
};

} // namespace editor
