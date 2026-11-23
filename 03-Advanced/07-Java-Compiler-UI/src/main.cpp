#include "CompilerToolchain.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "=========================================================\n";
    std::cout << "       C PLUS PLUS COMPILER TOOLCHAIN UI & RUNNER        \n";
    std::cout << "=========================================================\n";

    compiler_ui::CompilerToolchain toolchain("g++");
    toolchain.addFlag("-std=c++17");
    toolchain.addFlag("-Wall");
    toolchain.addFlag("-Wextra");

    std::cout << "Configured C++ Compiler Toolchain : " << toolchain.getCompilerPath() << "\n";
    std::cout << "Compiler Flags        : ";
    for (const auto& flag : toolchain.getFlags()) {
        std::cout << flag << " ";
    }
    std::cout << "\n\n";

    if (testRun) {
        std::cout << "[Test-Run Mode] Executing simulated C++ compilation...\n";
        auto res = toolchain.simulateCompile("int main() { syntax_error; return 0; }");

        std::cout << "Compilation Success   : " << (res.success ? "YES" : "NO") << "\n";
        std::cout << "Exit Code             : " << res.exitCode << "\n";
        std::cout << "Diagnostics Detected  : " << res.diagnostics.size() << "\n";

        for (const auto& d : res.diagnostics) {
            std::cout << "  [" << compiler_ui::DiagnosticParser::severityToString(d.severity) << "] "
                      << d.filePath << ":" << d.line << " -> " << d.message << "\n";
        }

        std::cout << "[Test-Run Mode] Executed successfully.\n";
        return 0;
    }

    std::cout << "Run with --test-run to execute automated diagnostic test.\n";
    return 0;
}
