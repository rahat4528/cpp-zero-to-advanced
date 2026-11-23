#include "CompilerToolchain.hpp"
#include <cstdio>
#include <sstream>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif

namespace compiler_ui {

CompilerToolchain::CompilerToolchain(const std::string& compilerPath)
    : m_compilerPath(compilerPath) {}

void CompilerToolchain::setCompilerPath(const std::string& path) {
    m_compilerPath = path;
}

const std::string& CompilerToolchain::getCompilerPath() const noexcept {
    return m_compilerPath;
}

void CompilerToolchain::addFlag(const std::string& flag) {
    m_flags.push_back(flag);
}

void CompilerToolchain::clearFlags() {
    m_flags.clear();
}

const std::vector<std::string>& CompilerToolchain::getFlags() const noexcept {
    return m_flags;
}

CompilationResult CompilerToolchain::compileFile(const std::string& sourcePath) const {
    CompilationResult result;

    std::ostringstream cmd;
    cmd << m_compilerPath;
    for (const auto& flag : m_flags) {
        cmd << " " << flag;
    }
    cmd << " " << sourcePath << " 2>&1";

    FILE* pipe = POPEN(cmd.str().c_str(), "r");
    if (!pipe) {
        result.exitCode = -1;
        result.stderrOutput = "Failed to launch compiler process.";
        result.success = false;
        return result;
    }

    char buffer[256];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int status = PCLOSE(pipe);
    result.exitCode = status;
    result.stdoutOutput = output;
    result.success = (status == 0);
    result.diagnostics = DiagnosticParser::parseOutput(output);

    return result;
}

CompilationResult CompilerToolchain::simulateCompile(const std::string& mockSource) const {
    CompilationResult result;
    if (mockSource.find("syntax_error") != std::string::npos) {
        result.exitCode = 1;
        result.success = false;
        std::ostringstream fakeDiag;
        fakeDiag << "Main.cpp:5:10: error: 'syntax_error' was not declared in this scope\n";
        fakeDiag << "Main.cpp:8:5: warning: unused variable 'temp' [-Wunused-variable]\n";
        result.stderrOutput = fakeDiag.str();
        result.diagnostics = DiagnosticParser::parseOutput(result.stderrOutput);
    } else {
        result.exitCode = 0;
        result.success = true;
        std::ostringstream fakeDiag;
        fakeDiag << "Main.cpp:8:5: warning: unused variable 'temp' [-Wunused-variable]\n";
        result.stdoutOutput = "Compilation succeeded with 0 errors.";
        result.stderrOutput = fakeDiag.str();
        result.diagnostics = DiagnosticParser::parseOutput(result.stderrOutput);
    }
    return result;
}

} // namespace compiler_ui
