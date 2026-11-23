#include "QuizTimerEngine.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    quiz_timer::QuizTimerEngine engine;

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "                TIMED TECHNICAL QUIZ ENGINE (TUI)                \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Simulating timed answering sequence...\n";

        engine.startQuiz();
        // Question 1: Answered correctly in 3.2 seconds
        engine.answerCurrentQuestion(1, 3.2);
        // Question 2: Answered correctly in 1.5 seconds (high speed bonus)
        engine.answerCurrentQuestion(0, 1.5);
        // Question 3: Answered wrong in 4.0 seconds
        engine.answerCurrentQuestion(0, 4.0);
        // Question 4: Timed out in 12.0 seconds (limit was 10s)
        engine.answerCurrentQuestion(2, 12.0);

        auto summary = engine.calculateSummary();
        std::cout << "\nQUIZ PERFORMANCE AUDIT:\n";
        std::cout << "  - Total Questions  : " << summary.totalQuestions << "\n";
        std::cout << "  - Correct Answers  : " << summary.correctCount << "\n";
        std::cout << "  - Timed Out        : " << summary.timedOutCount << "\n";
        std::cout << "  - Total Score Earned: " << summary.totalScore << " / " << summary.maxPossibleScore
                  << " (" << std::fixed << std::setprecision(1) << summary.scorePercentage << "%)\n";
        std::cout << "  - Avg Response Time : " << summary.averageResponseTimeSeconds << " s\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "                TIMED TECHNICAL QUIZ ENGINE (TUI)                \n";
    std::cout << "=================================================================\n";
    std::cout << "Run with --test-run for automated verification mode.\n";
    return 0;
}
