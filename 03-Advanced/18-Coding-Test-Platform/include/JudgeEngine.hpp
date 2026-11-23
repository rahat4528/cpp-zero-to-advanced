#pragma once

#include "ExecutionSandbox.hpp"
#include <string>
#include <vector>

namespace coding_judge {

enum class Verdict {
    Accepted,
    WrongAnswer,
    TimeLimitExceeded,
    MemoryLimitExceeded,
    RuntimeError,
    CompilationError
};

struct TestCase {
    int id{1};
    std::string input;
    std::string expectedOutput;
};

struct TestVerdict {
    int testCaseId{1};
    Verdict verdict{Verdict::Accepted};
    uint64_t timeMs{0};
    size_t memoryBytes{0};
    std::string actualOutput;
    std::string message;
};

struct SubmissionReport {
    Verdict overallVerdict{Verdict::Accepted};
    std::string compilerOutput;
    std::vector<TestVerdict> testVerdicts;
    uint64_t maxTimeMs{0};
    size_t peakMemoryBytes{0};
};

class JudgeEngine {
public:
    explicit JudgeEngine(uint64_t defaultTimeLimitMs = 1500, size_t defaultMemoryLimitBytes = 67108864);

    [[nodiscard]] static std::string verdictToString(Verdict v);
    [[nodiscard]] static bool compareOutputs(const std::string& actual, const std::string& expected);

    [[nodiscard]] SubmissionReport evaluateSubmission(
        const std::string& sourceCode,
        const std::vector<TestCase>& testCases,
        const std::string& compilerExecutable = "g++"
    );

    [[nodiscard]] SubmissionReport simulateEvaluation(
        const std::string& submissionType,
        const std::vector<TestCase>& testCases
    );

private:
    uint64_t m_timeLimitMs;
    size_t m_memoryLimitBytes;
};

} // namespace coding_judge
