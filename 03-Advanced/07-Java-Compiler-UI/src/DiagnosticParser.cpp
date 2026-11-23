#include "DiagnosticParser.hpp"
#include <sstream>
#include <regex>

namespace compiler_ui {

std::string DiagnosticParser::severityToString(DiagnosticSeverity sev) {
    switch (sev) {
        case DiagnosticSeverity::Warning: return "WARNING";
        case DiagnosticSeverity::Error:   return "ERROR";
        case DiagnosticSeverity::Info:
        default:                          return "INFO";
    }
}

std::vector<Diagnostic> DiagnosticParser::parseOutput(const std::string& rawOutput) {
    std::vector<Diagnostic> list;
    std::istringstream iss(rawOutput);
    std::string line;

    // GCC/Clang diagnostic pattern: File.cpp:14:5: error: message
    // MSVC diagnostic pattern: File.cpp(14): error C2065: message
    const std::regex gccRegex(R"(^([^:\s]+):(\d+):(\d+):\s*(error|warning|note):\s*(.+)$)");
    const std::regex msvcRegex(R"(^([^(]+)\((\d+)\):\s*(error|warning|fatal error)\s*([A-Za-z0-9]+):\s*(.+)$)");
    const std::regex simpleRegex(R"(^([^:\s]+):(\d+):\s*(error|warning|note):\s*(.+)$)");

    while (std::getline(iss, line)) {
        std::smatch match;
        if (std::regex_match(line, match, gccRegex)) {
            Diagnostic diag;
            diag.filePath = match[1].str();
            diag.line = std::stoi(match[2].str());
            diag.column = std::stoi(match[3].str());

            std::string sev = match[4].str();
            if (sev == "error") diag.severity = DiagnosticSeverity::Error;
            else if (sev == "warning") diag.severity = DiagnosticSeverity::Warning;
            else diag.severity = DiagnosticSeverity::Info;

            diag.message = match[5].str();
            list.push_back(diag);
        } else if (std::regex_match(line, match, msvcRegex)) {
            Diagnostic diag;
            diag.filePath = match[1].str();
            diag.line = std::stoi(match[2].str());
            diag.column = 0;

            std::string sev = match[3].str();
            if (sev.find("error") != std::string::npos) diag.severity = DiagnosticSeverity::Error;
            else diag.severity = DiagnosticSeverity::Warning;

            diag.message = match[4].str() + ": " + match[5].str();
            list.push_back(diag);
        } else if (std::regex_match(line, match, simpleRegex)) {
            Diagnostic diag;
            diag.filePath = match[1].str();
            diag.line = std::stoi(match[2].str());
            diag.column = 0;

            std::string sev = match[3].str();
            if (sev == "error") diag.severity = DiagnosticSeverity::Error;
            else if (sev == "warning") diag.severity = DiagnosticSeverity::Warning;
            else diag.severity = DiagnosticSeverity::Info;

            diag.message = match[4].str();
            list.push_back(diag);
        }
    }

    return list;
}

} // namespace compiler_ui
