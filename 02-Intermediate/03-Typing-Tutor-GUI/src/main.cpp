#include "TypingTutor.hpp"
#include <iostream>
#include <iomanip>
#include <string>

void printHeader() {
    std::cout << "=================================================================\n";
    std::cout << "              INTERACTIVE TYPING TUTOR GUI / ENGINE               \n";
    std::cout << "=================================================================\n";
}

void printCurriculum(const typing_tutor::TypingTutorEngine& engine) {
    std::cout << "\nAVAILABLE LESSON MODULES:\n";
    std::cout << "-----------------------------------------------------------------\n";
    for (const auto& l : engine.getCurriculum()) {
        std::cout << " [" << l.id << "] " << std::left << std::setw(28) << l.title
                  << " Category: " << std::setw(15) << l.category
                  << " Target: " << l.targetWpm << " WPM / " << l.targetAccuracy << "%\n";
    }
    std::cout << "-----------------------------------------------------------------\n";
}

void printResultDashboard(const typing_tutor::TutorStats& stats, const typing_tutor::Lesson& lesson) {
    std::cout << "\n=================================================================\n";
    std::cout << "                      LESSON PERFORMANCE AUDIT                   \n";
    std::cout << "=================================================================\n";
    std::cout << " Target: " << lesson.targetWpm << " WPM | " << lesson.targetAccuracy << "% Accuracy\n";
    std::cout << " Status: " << (stats.targetPassed ? "PASSED (COMPLETED)" : "NEEDS PRACTICE") << "\n";
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << " Elapsed Time   : " << std::fixed << std::setprecision(2) << stats.elapsedTimeSeconds << " s\n";
    std::cout << " Total Keystrokes: " << stats.totalTyped << " (" << stats.correctCount << " correct, " << stats.errorCount << " errors)\n";
    std::cout << " Accuracy       : " << std::fixed << std::setprecision(1) << stats.accuracyPct << " %\n";
    std::cout << " Gross Speed    : " << std::fixed << std::setprecision(1) << stats.grossWpm << " WPM\n";
    std::cout << " Net Speed      : " << std::fixed << std::setprecision(1) << stats.netWpm << " WPM\n";

    if (!stats.errorDistribution.empty()) {
        std::cout << "\n Key Mistake Distribution:\n";
        for (const auto& [ch, cnt] : stats.errorDistribution) {
            char displayChar = (ch == ' ') ? '_' : ch;
            std::cout << "   ['" << displayChar << "']: " << cnt << " mistake(s)\n";
        }
    }
    std::cout << "=================================================================\n";
}

int main(int argc, char* argv[]) {
    typing_tutor::TypingTutorEngine engine;

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        printHeader();
        engine.startLesson(1);
        std::cout << "[Test-Run Mode] Simulating student typing for Lesson 1...\n";
        const auto* l = engine.getCurrentLesson();
        for (char c : l->content) {
            engine.processKey(c);
        }
        auto stats = engine.calculateStats();
        printResultDashboard(stats, *l);
        return 0;
    }

    printHeader();
    printCurriculum(engine);

    std::cout << "\nSelect a lesson ID to begin (or 0 to exit): ";
    int choice = 0;
    if (!(std::cin >> choice) || choice == 0) {
        std::cout << "Exiting Typing Tutor.\n";
        return 0;
    }

    try {
        engine.startLesson(choice);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    const auto* lesson = engine.getCurrentLesson();
    std::cout << "\n-----------------------------------------------------------------\n";
    std::cout << "LESSON: " << lesson->title << "\n";
    std::cout << "TYPE THE FOLLOWING TEXT EXACTLY AS SHOWN (followed by Enter):\n";
    std::cout << "-----------------------------------------------------------------\n\n";
    std::cout << lesson->content << "\n\n";
    std::cout << "> ";

    std::string userInput;
    std::cin.ignore();
    std::getline(std::cin, userInput);

    for (char c : userInput) {
        engine.processKey(c);
    }

    auto stats = engine.calculateStats();
    printResultDashboard(stats, *lesson);

    return 0;
}
