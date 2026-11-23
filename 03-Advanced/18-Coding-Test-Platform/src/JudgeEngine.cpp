#include "JudgeEngine.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace coding_judge {

JudgeEngine::JudgeEngine(uint64_t defaultTimeLimitMs, size_t defaultMemoryLimitBytes)
    : m_timeLimitMs(defaultTimeLimitMs), m_memoryLimitBytes(defaultMemoryLimitBytes) {}

std::string JudgeEngine::verdictToString(Verdict v) {
    switch (v) {
        case Verdict::Accepted:            return "AC (Accepted)";
        case Verdict::WrongAnswer:         return "WA (Wrong Answer)";
        case Verdict::TimeLimitExceeded:   return "TLE (Time Limit Exceeded)";
        case Verdict::MemoryLimitExceeded: return "MLE (Memory Limit Exceeded)";
        case Verdict::RuntimeError:        return "RE (Runtime Error)";
        case Verdict::CompilationError:
        default:                           return "CE (Compilation Error)";
    }
}

static std::string trimWhitespace(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

bool JudgeEngine::compareOutputs(const std::string& actual, const std::string& expected) {
    std::istringstream issA(actual);
    std::istringstream issE(expected);
    std::string lineA, lineE;

    while (true) {
        bool hasA = static_cast<bool>(std::getline(issA, lineA));
        bool hasE = static_cast<bool>(std::getline(issE, lineE));

        if (!hasA && !hasE) break;

        std::string trimA = hasA ? trimWhitespace(lineA) : "";
        std::string trimE = hasE ? trimWhitespace(lineE) : "";

        if (trimA != trimE) return false;
    }
    return true;
}

SubmissionReport JudgeEngine::simulateEvaluation(
    const std::string& submissionType,
    const std::vector<TestCase>& testCases
) {
    SubmissionReport rep;
    if (submissionType == "CE") {
        rep.overallVerdict = Verdict::CompilationError;
        rep.compilerOutput = "submission.cpp:4:1: error: expected ';' before '}' token";
        return rep;
    }

    rep.overallVerdict = Verdict::Accepted;
    for (const auto& tc : testCases) {
        TestVerdict tv;
        tv.testCaseId = tc.id;
        tv.timeMs = 12;
        tv.memoryBytes = 4096 * 1024;

        if (submissionType == "AC") {
            tv.verdict = Verdict::Accepted;
            tv.actualOutput = tc.expectedOutput;
        } else if (submissionType == "WA") {
            tv.verdict = Verdict::WrongAnswer;
            tv.actualOutput = "WrongAnswerPayload42\n";
            rep.overallVerdict = Verdict::WrongAnswer;
        } else if (submissionType == "TLE") {
            tv.verdict = Verdict::TimeLimitExceeded;
            tv.timeMs = m_timeLimitMs + 50;
            rep.overallVerdict = Verdict::TimeLimitExceeded;
        }
        rep.testVerdicts.push_back(tv);
        rep.maxTimeMs = std::max(rep.maxTimeMs, tv.timeMs);
        rep.peakMemoryBytes = std::max(rep.peakMemoryBytes, tv.memoryBytes);
    }
    return rep;
}

SubmissionReport JudgeEngine::evaluateSubmission(
    const std::string& sourceCode,
    const std::vector<TestCase>& testCases,
    const std::string& compilerExecutable
) {
    SubmissionReport report;
    std::string srcFile = "temp_judge_submission.cpp";
    std::string exeFile = "temp_judge_submission.exe";

    {
        std::ofstream out(srcFile);
        out << sourceCode;
    }

    std::string compileCmd = compilerExecutable + " -O2 -std=c++17 " + srcFile + " -o " + exeFile + " 2>&1";
    auto compileRes = ExecutionSandbox::runProcess(compileCmd, "", 10000, 0);

    if (compileRes.exitCode != 0 || !std::filesystem::exists(exeFile)) {
        report.overallVerdict = Verdict::CompilationError;
        report.compilerOutput = compileRes.stdoutData;
        std::filesystem::remove(srcFile);
        return report;
    }

    report.overallVerdict = Verdict::Accepted;

    for (const auto& tc : testCases) {
        TestVerdict tv;
        tv.testCaseId = tc.id;

        auto metrics = ExecutionSandbox::runProcess(exeFile, tc.input, m_timeLimitMs, m_memoryLimitBytes);
        tv.timeMs = metrics.executionTimeMs;
        tv.memoryBytes = metrics.peakMemoryBytes;
        tv.actualOutput = metrics.stdoutData;

        report.maxTimeMs = std::max(report.maxTimeMs, tv.timeMs);
        report.peakMemoryBytes = std::max(report.peakMemoryBytes, tv.memoryBytes);

        if (metrics.timedOut) {
            tv.verdict = Verdict::TimeLimitExceeded;
        } else if (metrics.memoryExceeded) {
            tv.verdict = Verdict::MemoryLimitExceeded;
        } else if (metrics.exitCode != 0) {
            tv.verdict = Verdict::RuntimeError;
        } else if (compareOutputs(metrics.stdoutData, tc.expectedOutput)) {
            tv.verdict = Verdict::Accepted;
        } else {
            tv.verdict = Verdict::WrongAnswer;
        }

        if (report.overallVerdict == Verdict::Accepted && tv.verdict != Verdict::Accepted) {
            report.overallVerdict = tv.verdict;
        }

        report.testVerdicts.push_back(tv);
    }

    std::filesystem::remove(srcFile);
    std::filesystem::remove(exeFile);

    return report;
}

} // namespace coding_judge
