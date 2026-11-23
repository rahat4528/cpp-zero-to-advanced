#include "CppSyntaxHighlighter.hpp"
#include <unordered_set>
#include <cctype>
#include <sstream>

namespace editor {

bool CppSyntaxHighlighter::isKeyword(const std::string& word) {
    static const std::unordered_set<std::string> keywords = {
        "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
        "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor",
        "bool", "break", "case", "catch", "char", "char8_t", "char16_t",
        "char32_t", "class", "compl", "concept", "const", "consteval",
        "constexpr", "constinit", "const_cast", "continue", "co_await",
        "co_return", "co_yield", "decltype", "default", "delete", "do",
        "double", "dynamic_cast", "else", "enum", "explicit", "export",
        "extern", "false", "float", "for", "friend", "goto", "if",
        "inline", "int", "long", "mutable", "namespace", "new", "noexcept",
        "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
        "protected", "public", "reflexpr", "register", "reinterpret_cast",
        "requires", "return", "short", "signed", "sizeof", "static",
        "static_assert", "static_cast", "struct", "switch", "synchronized",
        "template", "this", "thread_local", "throw", "true", "try",
        "typedef", "typeid", "typename", "union", "unsigned", "using",
        "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
    };
    return keywords.find(word) != keywords.end();
}

bool CppSyntaxHighlighter::isPrimitiveType(const std::string& word) {
    static const std::unordered_set<std::string> primitives = {
        "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t",
        "uint32_t", "uint64_t", "size_t", "ptrdiff_t", "intptr_t", "uintptr_t",
        "string", "string_view", "vector", "map", "set", "unordered_map",
        "unordered_set", "array", "deque", "list", "pair", "tuple", "optional"
    };
    return primitives.find(word) != primitives.end();
}

std::vector<SyntaxToken> CppSyntaxHighlighter::tokenizeLine(const std::string& line) {
    std::vector<SyntaxToken> tokens;
    size_t i = 0;
    const size_t len = line.length();

    while (i < len) {
        // Whitespace
        if (std::isspace(static_cast<unsigned char>(line[i]))) {
            size_t start = i;
            while (i < len && std::isspace(static_cast<unsigned char>(line[i]))) i++;
            tokens.push_back({TokenType::Whitespace, line.substr(start, i - start)});
            continue;
        }

        // Single-line comment
        if (i + 1 < len && line[i] == '/' && line[i + 1] == '/') {
            tokens.push_back({TokenType::Comment, line.substr(i)});
            break;
        }

        // Preprocessor directive (#include, #define, #pragma, etc.)
        if (line[i] == '#') {
            size_t start = i++;
            while (i < len && (std::isalnum(static_cast<unsigned char>(line[i])) || line[i] == '_')) i++;
            tokens.push_back({TokenType::Preprocessor, line.substr(start, i - start)});
            continue;
        }

        // String literal
        if (line[i] == '"') {
            size_t start = i++;
            while (i < len && line[i] != '"') {
                if (line[i] == '\\' && i + 1 < len) i += 2;
                else i++;
            }
            if (i < len && line[i] == '"') i++;
            tokens.push_back({TokenType::StringLiteral, line.substr(start, i - start)});
            continue;
        }

        // Char literal
        if (line[i] == '\'') {
            size_t start = i++;
            while (i < len && line[i] != '\'') {
                if (line[i] == '\\' && i + 1 < len) i += 2;
                else i++;
            }
            if (i < len && line[i] == '\'') i++;
            tokens.push_back({TokenType::CharLiteral, line.substr(start, i - start)});
            continue;
        }

        // Numbers
        if (std::isdigit(static_cast<unsigned char>(line[i]))) {
            size_t start = i;
            while (i < len && (std::isalnum(static_cast<unsigned char>(line[i])) || line[i] == '.' || line[i] == '\'')) i++;
            tokens.push_back({TokenType::NumberLiteral, line.substr(start, i - start)});
            continue;
        }

        // Identifiers and keywords
        if (std::isalpha(static_cast<unsigned char>(line[i])) || line[i] == '_') {
            size_t start = i;
            while (i < len && (std::isalnum(static_cast<unsigned char>(line[i])) || line[i] == '_')) i++;
            std::string word = line.substr(start, i - start);

            if (isKeyword(word)) {
                tokens.push_back({TokenType::Keyword, word});
            } else if (isPrimitiveType(word)) {
                tokens.push_back({TokenType::PrimitiveType, word});
            } else {
                tokens.push_back({TokenType::Identifier, word});
            }
            continue;
        }

        // Punctuation / Operators
        tokens.push_back({TokenType::Punctuation, std::string(1, line[i])});
        i++;
    }

    return tokens;
}

std::string CppSyntaxHighlighter::renderAnsiColoredLine(const std::string& line) {
    auto tokens = tokenizeLine(line);
    std::ostringstream oss;

    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::Keyword:
                oss << "\033[1;34m" << token.text << "\033[0m"; // Bold Blue
                break;
            case TokenType::PrimitiveType:
                oss << "\033[1;36m" << token.text << "\033[0m"; // Bold Cyan
                break;
            case TokenType::StringLiteral:
            case TokenType::CharLiteral:
                oss << "\033[32m" << token.text << "\033[0m";   // Green
                break;
            case TokenType::NumberLiteral:
                oss << "\033[33m" << token.text << "\033[0m";   // Yellow
                break;
            case TokenType::Comment:
                oss << "\033[90m" << token.text << "\033[0m";   // Dark Gray
                break;
            case TokenType::Preprocessor:
                oss << "\033[35m" << token.text << "\033[0m";   // Magenta
                break;
            case TokenType::Identifier:
            case TokenType::Punctuation:
            case TokenType::Whitespace:
            default:
                oss << token.text;
                break;
        }
    }

    return oss.str();
}

} // namespace editor
