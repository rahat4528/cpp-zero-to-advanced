#include "ChatbotClient.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    chatbot::ChatbotClient bot;

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "             LLM AI CHATBOT TOOL (API & MOCK ENGINE)             \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Querying assistant with test prompt...\n\n";

        std::string query = "What is the core advantage of RAII in C++?";
        std::cout << "User : " << query << "\n";
        auto resp = bot.sendMessage(query);

        std::cout << "AI   : " << resp.replyText << "\n";
        std::cout << "Stats: " << (resp.isMock ? "[MOCK MODE]" : "[LIVE API]")
                  << " | Prompt Tokens: " << resp.promptTokens
                  << " | Completion Tokens: " << resp.completionTokens << "\n";

        std::cout << "\n[Test-Run Mode] Completed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "             LLM AI CHATBOT TOOL (API & MOCK ENGINE)             \n";
    std::cout << "=================================================================\n";
    std::cout << "Interactive Chatbot initialized. Type '/exit' to quit.\n\n";

    std::string input;
    std::cout << "> ";
    while (std::getline(std::cin, input)) {
        if (input == "/exit" || input == "exit") break;
        if (!input.empty()) {
            auto resp = bot.sendMessage(input);
            std::cout << "\n[AI]: " << resp.replyText << "\n\n> ";
        }
    }
    return 0;
}
