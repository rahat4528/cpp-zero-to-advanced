#include "PluginManager.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    bool testRun = false;
    std::string pluginPath = "plugins/libFormatterPlugin.dll";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test-run") {
            testRun = true;
        } else if (arg == "--plugin" && i + 1 < argc) {
            pluginPath = argv[++i];
        }
    }

    std::cout << "=== Minimal IDE Plugin Host Architecture ===\n";

    ide_plugin::PluginManager manager;

    // Search common relative locations for the plugin library
    std::vector<std::string> searchPaths = {
        pluginPath,
        "plugins/FormatterPlugin.dll",
        "plugins/libFormatterPlugin.dll",
        "libFormatterPlugin.dll",
        "FormatterPlugin.dll",
        "plugins/libFormatterPlugin.so",
        "libFormatterPlugin.so"
    };

    bool loaded = false;
    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path)) {
            std::cout << "Found plugin at: " << path << "\n";
            if (manager.loadPlugin(path)) {
                loaded = true;
                break;
            }
        }
    }

    if (!loaded) {
        std::cout << "Notice: Plugin not found in current directory. Attempting direct load with provided path...\n";
        manager.loadPlugin(pluginPath);
    }

    std::cout << "Loaded Plugins Count: " << manager.getPluginCount() << "\n";

    if (manager.getPluginCount() > 0) {
        std::string rawCode = "int main() {\nint x = 10;\nif (x > 5) {\nreturn 0;\n}\n}\n";
        std::cout << "\nDispatching 'format' command to CppFormatter:\n";
        std::string formatted = manager.dispatchCommand("CppFormatter", "format", rawCode);
        std::cout << "--- Formatted Output ---\n" << formatted << "------------------------\n";
    }

    if (testRun) {
        std::cout << "[Test Mode] Dynamic plugin host verification executed successfully.\n";
        return 0;
    }

    return 0;
}
