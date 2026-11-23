#pragma once

#include <string>
#include <cstdint>

namespace coding_judge {

struct ExecutionMetrics {
    int exitCode{0};
    uint64_t executionTimeMs{0};
    size_t peakMemoryBytes{0};
    std::string stdoutData;
    std::string stderrData;
    bool timedOut{false};
    bool memoryExceeded{false};
};

class ExecutionSandbox {
public:
    [[nodiscard]] static ExecutionMetrics runProcess(
        const std::string& command,
        const std::string& stdinInput,
        uint64_t timeLimitMs,
        size_t memoryLimitBytes
    );
};

} // namespace coding_judge
