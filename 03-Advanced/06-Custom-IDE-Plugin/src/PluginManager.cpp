#include "PluginManager.hpp"
#include <iostream>

namespace ide_plugin {

void HostContext::logMessage(const std::string& level, const std::string& message) {
    std::string entry = "[" + level + "] " + message;
    m_logs.push_back(entry);
    std::cout << "[HostContext] " << entry << "\n";
}

void HostContext::registerCommand(const std::string& commandName, const std::string& description) {
    m_commands[commandName] = description;
    std::cout << "[HostContext] Registered command: '" << commandName << "' -> " << description << "\n";
}

PluginManager::PluginManager() = default;

PluginManager::~PluginManager() {
    unloadAll();
}

bool PluginManager::loadPlugin(const std::string& sharedLibPath) {
    auto lib = std::make_unique<DynamicLibrary>();
    if (!lib->load(sharedLibPath)) {
        m_context.logMessage("ERROR", "Could not load shared library: " + sharedLibPath);
        return false;
    }

    auto createFn = reinterpret_cast<CreatePluginFunc>(lib->getSymbol("createPlugin"));
    auto destroyFn = reinterpret_cast<DestroyPluginFunc>(lib->getSymbol("destroyPlugin"));

    if (!createFn || !destroyFn) {
        m_context.logMessage("ERROR", "Missing createPlugin or destroyPlugin entry point in " + sharedLibPath);
        return false;
    }

    IPlugin* instance = createFn();
    if (!instance) {
        m_context.logMessage("ERROR", "createPlugin() returned nullptr for " + sharedLibPath);
        return false;
    }

    if (!instance->initialize(&m_context)) {
        m_context.logMessage("ERROR", "Failed to initialize plugin instance: " + std::string(instance->getName()));
        destroyFn(instance);
        return false;
    }

    m_context.logMessage("INFO", "Successfully loaded plugin: " + std::string(instance->getName()) + " v" + instance->getVersion());

    LoadedPlugin lp;
    lp.library = std::move(lib);
    lp.instance = instance;
    lp.destroyFunc = destroyFn;
    m_plugins.push_back(std::move(lp));
    return true;
}

void PluginManager::unloadAll() {
    for (auto& p : m_plugins) {
        if (p.instance) {
            p.instance->shutdown();
            if (p.destroyFunc) {
                p.destroyFunc(p.instance);
            }
            p.instance = nullptr;
        }
        if (p.library) {
            p.library->unload();
            p.library.reset();
        }
    }
    m_plugins.clear();
}

size_t PluginManager::getPluginCount() const noexcept {
    return m_plugins.size();
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const {
    std::vector<std::string> names;
    for (const auto& p : m_plugins) {
        if (p.instance) {
            names.push_back(p.instance->getName());
        }
    }
    return names;
}

std::string PluginManager::dispatchCommand(const std::string& pluginName, const std::string& command, const std::string& args) {
    for (auto& p : m_plugins) {
        if (p.instance && pluginName == p.instance->getName()) {
            return p.instance->executeCommand(command, args);
        }
    }
    return "Error: Plugin '" + pluginName + "' not found.";
}

} // namespace ide_plugin
