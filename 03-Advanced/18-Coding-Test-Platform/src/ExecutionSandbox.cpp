#include "ExecutionSandbox.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <psapi.h>
#endif

namespace coding_judge {

ExecutionMetrics ExecutionSandbox::runProcess(
    const std::string& command,
    const std::string& stdinInput,
    uint64_t timeLimitMs,
    size_t memoryLimitBytes
) {
    ExecutionMetrics metrics;
    std::string tempInFile = "sandbox_stdin_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".tmp";
    std::string tempOutFile = "sandbox_stdout_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".tmp";

    {
        std::ofstream in(tempInFile, std::ios::binary);
        in.write(stdinInput.data(), static_cast<std::streamsize>(stdinInput.size()));
    }

    std::string fullCmd = command + " < " + tempInFile + " > " + tempOutFile + " 2>&1";

#if defined(_WIN32) || defined(_WIN64)
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    std::string cmdCopy = "cmd.exe /c " + fullCmd;

    auto startTime = std::chrono::steady_clock::now();

    if (!CreateProcessA(nullptr, cmdCopy.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        metrics.exitCode = -1;
        metrics.stderrData = "Failed to launch process";
        std::filesystem::remove(tempInFile);
        std::filesystem::remove(tempOutFile);
        return metrics;
    }

    bool active = true;
    while (active) {
        DWORD waitRes = WaitForSingleObject(pi.hProcess, 50);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime
        ).count();

        // Sample memory
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc))) {
            metrics.peakMemoryBytes = std::max<size_t>(metrics.peakMemoryBytes, pmc.PeakWorkingSetSize);
            if (memoryLimitBytes > 0 && metrics.peakMemoryBytes > memoryLimitBytes) {
                metrics.memoryExceeded = true;
                TerminateProcess(pi.hProcess, 1);
                break;
            }
        }

        if (waitRes == WAIT_OBJECT_0) {
            DWORD code = 0;
            GetExitCodeProcess(pi.hProcess, &code);
            metrics.exitCode = static_cast<int>(code);
            metrics.executionTimeMs = static_cast<uint64_t>(elapsed);
            active = false;
        } else if (timeLimitMs > 0 && static_cast<uint64_t>(elapsed) > timeLimitMs) {
            metrics.timedOut = true;
            metrics.executionTimeMs = static_cast<uint64_t>(elapsed);
            TerminateProcess(pi.hProcess, 1);
            active = false;
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
    // Fallback for non-Windows
    (void)memoryLimitBytes;
    auto startTime = std::chrono::steady_clock::now();
    int code = std::system(fullCmd.c_str());
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime
    ).count();
    metrics.exitCode = code;
    metrics.executionTimeMs = static_cast<uint64_t>(elapsed);
    if (timeLimitMs > 0 && metrics.executionTimeMs > timeLimitMs) {
        metrics.timedOut = true;
    }
#endif

    // Read captured stdout
    std::ifstream out(tempOutFile, std::ios::binary);
    if (out) {
        std::string line;
        while (std::getline(out, line)) {
            metrics.stdoutData += line + "\n";
        }
    }

    std::filesystem::remove(tempInFile);
    std::filesystem::remove(tempOutFile);

    return metrics;
}

} // namespace coding_judge
