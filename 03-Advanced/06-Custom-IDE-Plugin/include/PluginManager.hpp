#pragma once

#include "IPlugin.hpp"
#include "DynamicLibrary.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace ide_plugin {

struct LoadedPlugin {
    std::unique_ptr<DynamicLibrary> library;
    IPlugin* instance{nullptr};
    DestroyPluginFunc destroyFunc{nullptr};
};

class HostContext : public IPluginContext {
public:
    void logMessage(const std::string& level, const std::string& message) override;
    void registerCommand(const std::string& commandName, const std::string& description) override;

    [[nodiscard]] const std::vector<std::string>& getLogs() const noexcept { return m_logs; }
    [[nodiscard]] const std::map<std::string, std::string>& getCommands() const noexcept { return m_commands; }

private:
    std::vector<std::string> m_logs;
    std::map<std::string, std::string> m_commands;
};

class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    bool loadPlugin(const std::string& sharedLibPath);
    void unloadAll();
    [[nodiscard]] size_t getPluginCount() const noexcept;

    [[nodiscard]] std::vector<std::string> getLoadedPluginNames() const;
    [[nodiscard]] std::string dispatchCommand(const std::string& pluginName, const std::string& command, const std::string& args);

    HostContext& getContext() { return m_context; }
    [[nodiscard]] const HostContext& getContext() const { return m_context; }

private:
    HostContext m_context;
    std::vector<LoadedPlugin> m_plugins;
};

} // namespace ide_plugin
