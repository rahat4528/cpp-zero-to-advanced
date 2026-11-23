#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "TextBuffer.hpp"
#include "CppSyntaxHighlighter.hpp"

TEST_CASE("TextBuffer - Modifications and Undo/Redo") {
    editor::TextBuffer buf;
    CHECK(buf.getLineCount() == 1);
    CHECK(buf.getContent() == "");

    buf.insertText("Hello World");
    CHECK(buf.getContent() == "Hello World");
    CHECK(buf.getCharacterCount() == 11);

    buf.insertLine(1, "Second Line");
    CHECK(buf.getLineCount() == 2);
    CHECK(buf.getLines()[1] == "Second Line");

    // Undo line insertion
    buf.undo();
    CHECK(buf.getLineCount() == 1);
    CHECK(buf.getContent() == "Hello World");

    // Redo
    buf.redo();
    CHECK(buf.getLineCount() == 2);
    CHECK(buf.getLines()[1] == "Second Line");
}

TEST_CASE("CppSyntaxHighlighter - Lexical Token Classification") {
    using editor::TokenType;

    SUBCASE("Keywords and Types") {
        std::string line = "class MyClass : public BaseService {";
        auto tokens = editor::CppSyntaxHighlighter::tokenizeLine(line);

        CHECK(tokens[0].type == TokenType::Keyword);
        CHECK(tokens[0].text == "class");

        CHECK(tokens[2].type == TokenType::Identifier);
        CHECK(tokens[2].text == "MyClass");

        CHECK(tokens[6].type == TokenType::Keyword);
        CHECK(tokens[6].text == "public");
    }

    SUBCASE("String Literals and Numbers") {
        std::string line = "const std::string msg = \"Hello \\\"World\\\"\"; int x = 42;";
        auto tokens = editor::CppSyntaxHighlighter::tokenizeLine(line);

        bool foundString = false;
        bool foundNumber = false;
        for (const auto& t : tokens) {
            if (t.type == TokenType::StringLiteral && t.text.find("Hello") != std::string::npos) foundString = true;
            if (t.type == TokenType::NumberLiteral && t.text == "42") foundNumber = true;
        }
        CHECK(foundString == true);
        CHECK(foundNumber == true);
    }

    SUBCASE("Comments and Preprocessor") {
        std::string commentLine = "// This is a single line comment";
        auto tokens = editor::CppSyntaxHighlighter::tokenizeLine(commentLine);
        REQUIRE(!tokens.empty());
        CHECK(tokens[0].type == TokenType::Comment);

        std::string prepLine = "#include <vector>";
        auto pTokens = editor::CppSyntaxHighlighter::tokenizeLine(prepLine);
        REQUIRE(!pTokens.empty());
        CHECK(pTokens[0].type == TokenType::Preprocessor);
        CHECK(pTokens[0].text == "#include");
    }
}
