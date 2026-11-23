#include "TypingSpeedTest.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Typing Speed Test CLI\n"
              << "Usage:\n"
              << "  TypingSpeedTest                                  Interactive live typing test\n"
              << "  TypingSpeedTest --prompt <text> --input <typed> --time <sec>  Automated test mode\n"
              << "  TypingSpeedTest --help                           Show help\n";
}

void printReport(const typing_speed::TypingMetrics& m) {
    std::cout << "\n============================================\n"
              << "             TYPING TEST RESULTS            \n"
              << "============================================\n"
              << std::fixed << std::setprecision(1)
              << "Elapsed Time   : " << m.elapsedSeconds << " seconds\n"
              << "Characters     : " << m.typedLength << " typed (" << m.correctChars << " exact matches)\n"
              << "Edit Errors    : " << m.errorChars << " (Levenshtein distance)\n"
              << "Accuracy       : " << m.accuracyPct << " %\n"
              << "--------------------------------------------\n"
              << "Raw Speed      : " << m.rawWpm << " WPM\n"
              << "Net Speed      : " << m.netWpm << " WPM (Net Words Per Minute)\n"
              << "Rating Level   : " << m.grade << "\n"
              << "============================================\n";
}

int main(int argc, char* argv[]) {
    std::string customPrompt;
    std::string automatedInput;
    double automatedTime = 0.0;
    bool automated = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--prompt" && i + 1 < argc) {
            customPrompt = argv[++i];
        } else if (arg == "--input" && i + 1 < argc) {
            automatedInput = argv[++i];
            automated = true;
        } else if (arg == "--time" && i + 1 < argc) {
            automatedTime = std::stod(argv[++i]);
        }
    }

    if (automated) {
        if (customPrompt.empty()) {
            customPrompt = "The quick brown fox jumps over the lazy dog.";
        }
        auto m = typing_speed::TypingEvaluator::evaluate(customPrompt, automatedInput, automatedTime);
        printReport(m);
        return 0;
    }

    std::string prompt = customPrompt.empty() ? typing_speed::TypingEvaluator::getDefaultPrompts()[0] : customPrompt;

    std::cout << "========================================================\n";
    std::cout << "                   TYPING SPEED TEST                    \n";
    std::cout << "========================================================\n";
    std::cout << "Type the following sentence exactly, then press ENTER:\n\n";
    std::cout << "PROMPT: " << prompt << "\n\n";
    std::cout << "Ready? Press ENTER to begin timer...";
    std::string dummy;
    std::getline(std::cin, dummy);

    std::cout << "\nStart typing now:\n> ";
    auto start = std::chrono::steady_clock::now();
    std::string typed;
    std::getline(std::cin, typed);
    auto end = std::chrono::steady_clock::now();

    double seconds = std::chrono::duration<double>(end - start).count();
    auto metrics = typing_speed::TypingEvaluator::evaluate(prompt, typed, seconds);
    printReport(metrics);

    return 0;
}
