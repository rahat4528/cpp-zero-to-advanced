#pragma once

#include <string>
#include <stdexcept>

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    using lib_handle_t = HMODULE;
#else
    #include <dlfcn.h>
    using lib_handle_t = void*;
#endif

namespace ide_plugin {

class DynamicLibrary {
public:
    DynamicLibrary() = default;
    explicit DynamicLibrary(const std::string& path) {
        if (!load(path)) {
            throw std::runtime_error("Failed to load dynamic library: " + path);
        }
    }

    ~DynamicLibrary() {
        unload();
    }

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept : m_handle(other.m_handle), m_path(std::move(other.m_path)) {
        other.m_handle = nullptr;
    }

    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept {
        if (this != &other) {
            unload();
            m_handle = other.m_handle;
            m_path = std::move(other.m_path);
            other.m_handle = nullptr;
        }
        return *this;
    }

    bool load(const std::string& path) {
        unload();
        m_path = path;
#if defined(_WIN32) || defined(_WIN64)
        m_handle = LoadLibraryA(path.c_str());
#else
        m_handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
        return m_handle != nullptr;
    }

    void unload() {
        if (m_handle) {
#if defined(_WIN32) || defined(_WIN64)
            FreeLibrary(m_handle);
#else
            dlclose(m_handle);
#endif
            m_handle = nullptr;
        }
    }

    [[nodiscard]] bool isLoaded() const noexcept {
        return m_handle != nullptr;
    }

    [[nodiscard]] void* getSymbol(const std::string& symbolName) const {
        if (!m_handle) return nullptr;
#if defined(_WIN32) || defined(_WIN64)
        return reinterpret_cast<void*>(GetProcAddress(m_handle, symbolName.c_str()));
#else
        return dlsym(m_handle, symbolName.c_str());
#endif
    }

    [[nodiscard]] const std::string& getPath() const noexcept { return m_path; }

private:
    lib_handle_t m_handle{nullptr};
    std::string m_path;
};

} // namespace ide_plugin
