#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace explorer {

enum class SortCriteria {
    NameAsc,
    NameDesc,
    SizeAsc,
    SizeDesc
};

struct FileEntry {
    std::string filename;
    std::string fullPath;
    std::string extension;
    uintmax_t sizeBytes{0};
    std::string humanSize;
    bool isDirectory{false};
    bool isSymlink{false};
    std::string lastModified;
};

class FileExplorerEngine {
public:
    explicit FileExplorerEngine(std::filesystem::path rootPath = std::filesystem::current_path());

    bool navigateTo(const std::filesystem::path& newPath);
    bool navigateUp();
    [[nodiscard]] std::filesystem::path getCurrentPath() const;

    [[nodiscard]] std::vector<FileEntry> listEntries(const std::string& extensionFilter = "",
                                                     SortCriteria sort = SortCriteria::NameAsc) const;

    static uintmax_t calculateRecursiveDirectorySize(const std::filesystem::path& dirPath);
    static std::string formatBytes(uintmax_t bytes);

private:
    std::filesystem::path m_currentPath;
};

} // namespace explorer
