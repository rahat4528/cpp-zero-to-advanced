#pragma once

#include "DiagnosticParser.hpp"
#include <string>
#include <vector>

namespace compiler_ui {

struct CompilationResult {
    int exitCode{0};
    std::string stdoutOutput;
    std::string stderrOutput;
    std::vector<Diagnostic> diagnostics;
    bool success{false};
};

class CompilerToolchain {
public:
    explicit CompilerToolchain(const std::string& compilerPath = "g++");

    void setCompilerPath(const std::string& path);
    [[nodiscard]] const std::string& getCompilerPath() const noexcept;

    void addFlag(const std::string& flag);
    void clearFlags();
    [[nodiscard]] const std::vector<std::string>& getFlags() const noexcept;

    [[nodiscard]] CompilationResult compileFile(const std::string& sourcePath) const;
    [[nodiscard]] CompilationResult simulateCompile(const std::string& mockSource) const;

private:
    std::string m_compilerPath;
    std::vector<std::string> m_flags;
};

} // namespace compiler_ui
