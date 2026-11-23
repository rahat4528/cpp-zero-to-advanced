#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "CompilerToolchain.hpp"
#include "DiagnosticParser.hpp"

namespace {

using namespace compiler_ui;

TEST_CASE("Diagnostic Parser - Parsing GCC, Clang & Generic Diagnostic Patterns") {
    std::string sampleOutput =
        "App.cpp:5:10: error: 'syntax_error' was not declared in this scope\n"
        "    5 |   int x = syntax_error;\n"
        "      |          ^~~~~~~~~~~~\n"
        "App.cpp:8:5: warning: unused variable 'temp' [-Wunused-variable]\n"
        "    8 |   int temp = 0;\n"
        "      |       ^~~~\n";

    auto diags = DiagnosticParser::parseOutput(sampleOutput);

    REQUIRE(diags.size() == 2);

    CHECK(diags[0].filePath == "App.cpp");
    CHECK(diags[0].line == 5);
    CHECK(diags[0].column == 10);
    CHECK(diags[0].severity == DiagnosticSeverity::Error);
    CHECK(diags[0].message.find("'syntax_error' was not declared") != std::string::npos);

    CHECK(diags[1].filePath == "App.cpp");
    CHECK(diags[1].line == 8);
    CHECK(diags[1].column == 5);
    CHECK(diags[1].severity == DiagnosticSeverity::Warning);
    CHECK(diags[1].message.find("unused variable 'temp'") != std::string::npos);
}

TEST_CASE("Diagnostic Parser - MSVC Pattern Parsing") {
    std::string msvcOutput =
        "Source.cpp(12): error C2065: 'undefined_var': undeclared identifier\n"
        "Source.cpp(25): warning C4101: 'unused': unreferenced local variable\n";

    auto diags = DiagnosticParser::parseOutput(msvcOutput);

    REQUIRE(diags.size() == 2);
    CHECK(diags[0].filePath == "Source.cpp");
    CHECK(diags[0].line == 12);
    CHECK(diags[0].severity == DiagnosticSeverity::Error);

    CHECK(diags[1].filePath == "Source.cpp");
    CHECK(diags[1].line == 25);
    CHECK(diags[1].severity == DiagnosticSeverity::Warning);
}

TEST_CASE("Compiler Toolchain - Configuration and Simulation Mode") {
    CompilerToolchain tool("g++");
    CHECK(tool.getCompilerPath() == "g++");

    tool.addFlag("-std=c++17");
    tool.addFlag("-O2");
    CHECK(tool.getFlags().size() == 2);

    tool.setCompilerPath("clang++");
    CHECK(tool.getCompilerPath() == "clang++");

    // Simulation of failure
    auto failureRes = tool.simulateCompile("int main() { syntax_error; }");
    CHECK(!failureRes.success);
    CHECK(failureRes.exitCode == 1);
    CHECK(failureRes.diagnostics.size() >= 1);

    // Simulation of success
    auto successRes = tool.simulateCompile("int main() { return 0; }");
    CHECK(successRes.success);
    CHECK(successRes.exitCode == 0);
}

} // namespace
