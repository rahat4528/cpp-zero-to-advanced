#include "FileExplorer.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace explorer {

FileExplorerEngine::FileExplorerEngine(std::filesystem::path rootPath)
    : m_currentPath(std::move(rootPath)) {}

bool FileExplorerEngine::navigateTo(const std::filesystem::path& newPath) {
    std::error_code ec;
    if (std::filesystem::is_directory(newPath, ec) && !ec) {
        m_currentPath = std::filesystem::canonical(newPath, ec);
        if (ec) m_currentPath = newPath;
        return true;
    }
    return false;
}

bool FileExplorerEngine::navigateUp() {
    if (m_currentPath.has_parent_path()) {
        m_currentPath = m_currentPath.parent_path();
        return true;
    }
    return false;
}

std::filesystem::path FileExplorerEngine::getCurrentPath() const {
    return m_currentPath;
}

std::string FileExplorerEngine::formatBytes(uintmax_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double dBytes = static_cast<double>(bytes);

    while (dBytes >= 1024.0 && unitIndex < 4) {
        dBytes /= 1024.0;
        unitIndex++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << dBytes << " " << units[unitIndex];
    return oss.str();
}

uintmax_t FileExplorerEngine::calculateRecursiveDirectorySize(const std::filesystem::path& dirPath) {
    uintmax_t total = 0;
    std::error_code ec;
    if (!std::filesystem::is_directory(dirPath, ec) || ec) return 0;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath, std::filesystem::directory_options::skip_permission_denied, ec)) {
        if (ec) continue;
        if (entry.is_regular_file(ec) && !ec) {
            total += entry.file_size(ec);
        }
    }
    return total;
}

std::vector<FileEntry> FileExplorerEngine::listEntries(const std::string& extensionFilter, SortCriteria sort) const {
    std::vector<FileEntry> entries;
    std::error_code ec;

    if (!std::filesystem::exists(m_currentPath, ec) || ec) {
        return entries;
    }

    for (const auto& dirEntry : std::filesystem::directory_iterator(m_currentPath, ec)) {
        if (ec) continue;

        FileEntry item;
        item.filename = dirEntry.path().filename().string();
        item.fullPath = dirEntry.path().string();
        item.extension = dirEntry.path().extension().string();
        item.isDirectory = dirEntry.is_directory(ec);
        item.isSymlink = dirEntry.is_symlink(ec);

        if (!extensionFilter.empty() && !item.isDirectory) {
            if (item.extension != extensionFilter && ("." + item.extension) != extensionFilter) {
                continue;
            }
        }

        if (item.isDirectory) {
            item.sizeBytes = 0;
            item.humanSize = "<DIR>";
        } else {
            item.sizeBytes = dirEntry.file_size(ec);
            item.humanSize = formatBytes(item.sizeBytes);
        }

        entries.push_back(item);
    }

    // Sort entries: directories always first, then by criteria
    std::sort(entries.begin(), entries.end(), [sort](const FileEntry& a, const FileEntry& b) {
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory > b.isDirectory; // Dirs first
        }
        switch (sort) {
            case SortCriteria::NameAsc: return a.filename < b.filename;
            case SortCriteria::NameDesc: return a.filename > b.filename;
            case SortCriteria::SizeAsc: return a.sizeBytes < b.sizeBytes;
            case SortCriteria::SizeDesc: return a.sizeBytes > b.sizeBytes;
        }
        return a.filename < b.filename;
    });

    return entries;
}

} // namespace explorer
