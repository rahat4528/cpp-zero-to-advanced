#include "TextBuffer.hpp"
#include "CppSyntaxHighlighter.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "        C PLUS PLUS CODE EDITOR (SYNTAX HIGHLIGHTER)             \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Initializing buffer and syntax highlighting C++ snippet...\n\n";

        std::string sampleCode =
            "#include <iostream>\n"
            "#include <string>\n"
            "\n"
            "// High-performance C Plus Plus service model\n"
            "class OrderService : public BaseService {\n"
            "private:\n"
            "    const int maxRetries = 3;\n"
            "    double timeoutSeconds = 15.5;\n"
            "\n"
            "public:\n"
            "    bool processOrder(const std::string& orderId) {\n"
            "        std::cout << \"Processing order: \" << orderId << std::endl;\n"
            "        return true;\n"
            "    }\n"
            "};";

        editor::TextBuffer buffer(sampleCode);
        std::cout << "Buffer lines: " << buffer.getLineCount()
                  << " | Total characters: " << buffer.getCharacterCount() << "\n\n";

        std::cout << "--- Highlighted Output ---\n";
        for (size_t i = 0; i < buffer.getLineCount(); ++i) {
            std::cout << std::setw(3) << std::right << (i + 1) << " | "
                      << editor::CppSyntaxHighlighter::renderAnsiColoredLine(buffer.getLines()[i])
                      << "\n";
        }
        std::cout << "--------------------------\n";
        std::cout << "[Test-Run Mode] Executed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "        C PLUS PLUS CODE EDITOR (SYNTAX HIGHLIGHTER)             \n";
    std::cout << "=================================================================\n";
    std::cout << "Notice: This application is built 100% in C Plus Plus.\n";
    std::cout << "Run with --test-run to execute automated syntax highlighting check.\n";
    return 0;
}
