#pragma once

#include <string>

#if defined(_WIN32) || defined(_WIN64)
    #define PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
    #define PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
#endif

namespace ide_plugin {

class IPluginContext {
public:
    virtual ~IPluginContext() = default;
    virtual void logMessage(const std::string& level, const std::string& message) = 0;
    virtual void registerCommand(const std::string& commandName, const std::string& description) = 0;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    [[nodiscard]] virtual const char* getName() const noexcept = 0;
    [[nodiscard]] virtual const char* getVersion() const noexcept = 0;
    [[nodiscard]] virtual const char* getDescription() const noexcept = 0;

    virtual bool initialize(IPluginContext* context) = 0;
    virtual void shutdown() = 0;

    [[nodiscard]] virtual std::string executeCommand(const std::string& command, const std::string& args) = 0;
};

using CreatePluginFunc = IPlugin* (*)();
using DestroyPluginFunc = void (*)(IPlugin*);

} // namespace ide_plugin
