#pragma once

#include <string>
#include <vector>

namespace compiler_ui {

enum class DiagnosticSeverity {
    Info,
    Warning,
    Error
};

struct Diagnostic {
    std::string filePath;
    int line{0};
    int column{0};
    DiagnosticSeverity severity{DiagnosticSeverity::Info};
    std::string message;
};

class DiagnosticParser {
public:
    [[nodiscard]] static std::vector<Diagnostic> parseOutput(const std::string& rawOutput);
    [[nodiscard]] static std::string severityToString(DiagnosticSeverity sev);
};

} // namespace compiler_ui
