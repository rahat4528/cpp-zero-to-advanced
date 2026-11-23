#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "JudgeEngine.hpp"

using namespace coding_judge;

TEST_SUITE("Coding Test Platform and Sandbox Evaluation") {
    TEST_CASE("Output Diff Checking with Whitespace Tolerance") {
        CHECK(JudgeEngine::compareOutputs("42\n", "42\n"));
        CHECK(JudgeEngine::compareOutputs("  42  \r\n", "42\n"));
        CHECK(JudgeEngine::compareOutputs("Line 1\nLine 2\n", "Line 1\r\nLine 2\r\n"));
        CHECK_FALSE(JudgeEngine::compareOutputs("42\n", "43\n"));
    }

    TEST_CASE("Verdict Evaluation Scenarios") {
        JudgeEngine judge(1000, 64 * 1024 * 1024);
        std::vector<TestCase> testCases = {
            {1, "3 4\n", "7\n"},
            {2, "10 20\n", "30\n"}
        };

        // AC Simulation
        auto ac = judge.simulateEvaluation("AC", testCases);
        CHECK(ac.overallVerdict == Verdict::Accepted);
        CHECK(ac.testVerdicts.size() == 2);

        // WA Simulation
        auto wa = judge.simulateEvaluation("WA", testCases);
        CHECK(wa.overallVerdict == Verdict::WrongAnswer);

        // TLE Simulation
        auto tle = judge.simulateEvaluation("TLE", testCases);
        CHECK(tle.overallVerdict == Verdict::TimeLimitExceeded);
        CHECK(tle.maxTimeMs >= 1000);

        // CE Simulation
        auto ce = judge.simulateEvaluation("CE", testCases);
        CHECK(ce.overallVerdict == Verdict::CompilationError);
        CHECK_FALSE(ce.compilerOutput.empty());
    }
}
