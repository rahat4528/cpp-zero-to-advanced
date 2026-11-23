#include "IPlugin.hpp"
#include <string>
#include <sstream>
#include <algorithm>

namespace ide_plugin {

class FormatterPlugin : public IPlugin {
public:
    [[nodiscard]] const char* getName() const noexcept override { return "CppFormatter"; }
    [[nodiscard]] const char* getVersion() const noexcept override { return "1.0.0"; }
    [[nodiscard]] const char* getDescription() const noexcept override {
        return "Automatic C++ code indentation and brace alignment plugin.";
    }

    bool initialize(IPluginContext* context) override {
        if (!context) return false;
        context->logMessage("INFO", "CppFormatter initialized successfully.");
        context->registerCommand("format", "Indents and strips trailing whitespaces.");
        return true;
    }

    void shutdown() override {}

    [[nodiscard]] std::string executeCommand(const std::string& command, const std::string& args) override {
        if (command == "format") {
            std::istringstream iss(args);
            std::ostringstream oss;
            std::string line;
            int indent = 0;
            while (std::getline(iss, line)) {
                size_t first = line.find_first_not_of(" \t");
                if (first == std::string::npos) {
                    oss << "\n";
                    continue;
                }
                size_t last = line.find_last_not_of(" \t");
                std::string trimmed = line.substr(first, last - first + 1);

                if (!trimmed.empty() && trimmed.front() == '}') {
                    indent = std::max(0, indent - 1);
                }

                oss << std::string(static_cast<size_t>(indent * 4), ' ') << trimmed << "\n";

                if (!trimmed.empty() && trimmed.back() == '{') {
                    indent++;
                }
            }
            return oss.str();
        }
        return "Unknown command: " + command;
    }
};

} // namespace ide_plugin

PLUGIN_EXPORT ide_plugin::IPlugin* createPlugin() {
    return new ide_plugin::FormatterPlugin();
}

PLUGIN_EXPORT void destroyPlugin(ide_plugin::IPlugin* plugin) {
    delete plugin;
}
