#include "QuizGame.h"

#include <iostream>
#include <sstream>
#include <string>

void printHelp() {
    std::cout << "Quiz Game UI\n"
              << "Usage:\n"
              << "  QuizGame                             Interactive terminal quiz\n"
              << "  QuizGame --file <path>               Load specific question bank\n"
              << "  QuizGame --answers \"B,C,B,C\"         Automated non-interactive test run\n"
              << "  QuizGame --help                      Show help\n";
}

int main(int argc, char* argv[]) {
    std::string questionsPath = "resources/questions.txt";
    std::string automatedAnswers;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--file" && i + 1 < argc) {
            questionsPath = argv[++i];
        } else if (arg == "--answers" && i + 1 < argc) {
            automatedAnswers = argv[++i];
        }
    }

    quiz_game::QuizEngine engine;
    if (!engine.loadQuestionsFromFile(questionsPath)) {
        // Fallback embedded question
        quiz_game::Question fallback;
        fallback.prompt = "What does RAII stand for?";
        fallback.options = {"Resource Allocation", "Resource Acquisition Is Initialization", "Random Access", "Runtime Array"};
        fallback.correctOption = 'B';
        fallback.explanation = "RAII manages resources via destructor scope.";
        engine.addQuestion(fallback);
    }

    if (!automatedAnswers.empty()) {
        std::stringstream ss(automatedAnswers);
        std::string token;
        size_t qIdx = 0;
        while (std::getline(ss, token, ',') && qIdx < engine.questionCount()) {
            if (!token.empty()) {
                char ans = token.front();
                bool correct = engine.submitAnswer(qIdx, ans);
                std::cout << "Question " << (qIdx + 1) << " Answer: " << ans
                          << " -> " << (correct ? "CORRECT" : "INCORRECT") << "\n";
                qIdx++;
            }
        }
        auto res = engine.calculateResults();
        std::cout << "\nQuiz Score: " << res.totalScore << "/" << res.maxPossibleScore
                  << " (" << res.percentage << "%) Grade: " << res.grade << "\n";
        return 0;
    }

    std::cout << "====================================================\n";
    std::cout << "               C++ MASTERY QUIZ GAME                \n";
    std::cout << "====================================================\n\n";

    for (size_t i = 0; i < engine.questionCount(); ++i) {
        const auto& q = engine.getQuestion(i);
        std::cout << "Question " << (i + 1) << " of " << engine.questionCount() << ":\n";
        std::cout << q.prompt << "\n\n";

        char optLabel = 'A';
        for (const auto& opt : q.options) {
            std::cout << "  [" << optLabel++ << "] " << opt << "\n";
        }

        std::cout << "\nYour answer (A/B/C/D): ";
        char choice = ' ';
        if (!(std::cin >> choice)) break;

        bool correct = engine.submitAnswer(i, choice);
        if (correct) {
            std::cout << ">> Correct! (+10 pts)\n";
        } else {
            std::cout << ">> Incorrect. The correct answer was [" << q.correctOption << "].\n";
        }
        if (!q.explanation.empty()) {
            std::cout << ">> Explanation: " << q.explanation << "\n";
        }
        std::cout << "----------------------------------------------------\n";
    }

    auto res = engine.calculateResults();
    std::cout << "\n====================================================\n";
    std::cout << "                 FINAL QUIZ REPORT                  \n";
    std::cout << "====================================================\n";
    std::cout << "Questions Answered Correctly: " << res.correctAnswers << " / " << res.totalQuestions << "\n";
    std::cout << "Total Score                 : " << res.totalScore << " / " << res.maxPossibleScore << "\n";
    std::cout << "Percentage                  : " << res.percentage << "%\n";
    std::cout << "Final Evaluation            : " << res.grade << "\n";
    std::cout << "====================================================\n";

    return 0;
}
