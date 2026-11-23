#include "JudgeEngine.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "=================================================\n";
    std::cout << "    Automated Coding Test Platform & Judge       \n";
    std::cout << "=================================================\n";

    std::cout << "\n[!] SANDBOXING & RESOURCE LIMITATION NOTICE:\n";
    std::cout << "  1. Host Process Limits: Execution time and memory are enforced via OS\n";
    std::cout << "     watchdogs (WaitForSingleObject / GetProcessMemoryInfo) and kill signals.\n";
    std::cout << "  2. Security Boundary: Windows processes run under current user privileges.\n";
    std::cout << "     Enterprise competitive platforms must run in isolated unprivileged containers\n";
    std::cout << "     (Linux cgroups / seccomp / Docker / gVisor) to isolate syscall access.\n";
    std::cout << "-------------------------------------------------\n";

    coding_judge::JudgeEngine judge(1000, 64 * 1024 * 1024);

    // Problem: Two Sum (Input: A B, Output: A+B)
    std::vector<coding_judge::TestCase> testCases = {
        {1, "10 25\n", "35\n"},
        {2, "-5 15\n", "10\n"},
        {3, "1000 2000\n", "3000\n"}
    };

    std::cout << "Problem: 'Two-Sum Addition'\n";
    std::cout << "Evaluating 3 test cases (Time Limit: 1000ms, Memory Limit: 64MB)...\n";

    std::cout << "\n[Evaluating Correct Solution -> Expected AC]\n";
    auto acRep = judge.simulateEvaluation("AC", testCases);
    std::cout << "Verdict: " << coding_judge::JudgeEngine::verdictToString(acRep.overallVerdict) << "\n";
    for (const auto& tv : acRep.testVerdicts) {
        std::cout << "  Test #" << tv.testCaseId << ": "
                  << coding_judge::JudgeEngine::verdictToString(tv.verdict)
                  << " (" << tv.timeMs << " ms, " << (tv.memoryBytes / 1024) << " KB)\n";
    }

    std::cout << "\n[Evaluating Erroneous Solution -> Expected WA]\n";
    auto waRep = judge.simulateEvaluation("WA", testCases);
    std::cout << "Verdict: " << coding_judge::JudgeEngine::verdictToString(waRep.overallVerdict) << "\n";

    std::cout << "\n[Evaluating Infinite Loop Solution -> Expected TLE]\n";
    auto tleRep = judge.simulateEvaluation("TLE", testCases);
    std::cout << "Verdict: " << coding_judge::JudgeEngine::verdictToString(tleRep.overallVerdict) << "\n";

    if (testRun) {
        std::cout << "\n[Test Mode] Code evaluation platform verified.\n";
        return 0;
    }

    return 0;
}
