# Architecture Design: Custom IDE Plugin Engine

## 1. System Overview
The `Custom-IDE-Plugin` architecture provides a robust, cross-platform dynamic library loading engine (`LoadLibrary` on Windows, `dlopen` on POSIX) communicating across a strict C-ABI interface boundary to ensure ABI compatibility and runtime crash isolation.

```mermaid
flowchart LR
    Host["Host Application"] --> Loader["PluginLoader (LoadLibrary / dlopen)"]
    Loader --> ABI["Plugin C-ABI Boundary (extern \"C\")"]
    ABI --> SharedLib["Dynamic Library (.dll / .so)"]
    SharedLib --> VTable["Plugin Interface Virtual Methods"]
    VTable --> Features["Feature Hooks (Format, Lint, Analyze)"]
```

## 2. Component Architecture
- **`IPlugin Interface`**:
  ```cpp
  class IPlugin {
  public:
      virtual ~IPlugin() = default;
      virtual const char* getName() const noexcept = 0;
      virtual const char* getVersion() const noexcept = 0;
      virtual bool initialize(IHostContext* context) = 0;
      virtual std::string executeCommand(const std::string& cmd, const std::string& args) = 0;
      virtual void shutdown() = 0;
  };
  ```
- **`C-ABI Factory Functions`**:
  Dynamic libraries export two non-mangled C entry points:
  - `extern "C" IPlugin* create_plugin();`
  - `extern "C" void destroy_plugin(IPlugin* plugin);`
- **`PluginManager`**:
  - Scans directories for `.dll` / `.so` libraries.
  - Verifies ABI compatibility via version handshake.
  - Manages plugin lifecycle (load, execute, unload) safely.

## 3. Crash Isolation & Security
- Plugins operate against an abstract `IHostContext` interface preventing direct memory corruption of the host engine.
- Shared libraries are unloaded deterministically in reverse registration order via RAII wrappers.
