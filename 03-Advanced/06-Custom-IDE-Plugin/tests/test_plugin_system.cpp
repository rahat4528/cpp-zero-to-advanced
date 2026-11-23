#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "PluginManager.hpp"
#include <filesystem>

using namespace ide_plugin;

TEST_SUITE("Custom IDE Dynamic Plugin System") {
    TEST_CASE("HostContext Command and Log Registration") {
        HostContext ctx;
        ctx.logMessage("INFO", "Host starting up");
        ctx.registerCommand("save_all", "Saves all open editor buffers");

        CHECK(ctx.getLogs().size() == 1);
        CHECK(ctx.getCommands().find("save_all") != ctx.getCommands().end());
    }

    TEST_CASE("DynamicLibrary Nonexistent File Graceful Handling") {
        DynamicLibrary lib;
        CHECK_FALSE(lib.load("non_existent_library_12345.dll"));
        CHECK_FALSE(lib.isLoaded());
        CHECK(lib.getSymbol("anySymbol") == nullptr);
    }

    TEST_CASE("Plugin Loading and Command Execution") {
        PluginManager pm;

        std::vector<std::string> potentialPaths = {
            "plugins/libFormatterPlugin.dll",
            "plugins/FormatterPlugin.dll",
            "libFormatterPlugin.dll",
            "FormatterPlugin.dll",
            "plugins/libFormatterPlugin.so",
            "libFormatterPlugin.so"
        };

        std::string foundPath;
        for (const auto& path : potentialPaths) {
            if (std::filesystem::exists(path)) {
                foundPath = path;
                break;
            }
        }

        if (!foundPath.empty()) {
            REQUIRE(pm.loadPlugin(foundPath));
            CHECK(pm.getPluginCount() == 1);

            auto names = pm.getLoadedPluginNames();
            REQUIRE(names.size() == 1);
            CHECK(names[0] == "CppFormatter");

            std::string unformatted = "int main() {\nint a = 1;\nreturn 0;\n}\n";
            std::string formatted = pm.dispatchCommand("CppFormatter", "format", unformatted);
            CHECK(formatted.find("    int a = 1;") != std::string::npos);

            pm.unloadAll();
            CHECK(pm.getPluginCount() == 0);
        }
    }
}
