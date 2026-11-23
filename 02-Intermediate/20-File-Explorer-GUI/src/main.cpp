#include "FileExplorer.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    explorer::FileExplorerEngine engine;

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "           FILE SYSTEM EXPLORER & METRICS INSPECTOR              \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Navigating local directory structure...\n";

        std::cout << "Current Directory: " << engine.getCurrentPath().string() << "\n\n";

        auto entries = engine.listEntries("", explorer::SortCriteria::NameAsc);
        std::cout << "Directory Contents (" << entries.size() << " items):\n";
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << std::left << std::setw(35) << "NAME"
                  << std::setw(12) << "TYPE"
                  << std::setw(15) << "SIZE" << "\n";
        std::cout << "-----------------------------------------------------------------\n";

        for (const auto& item : entries) {
            std::cout << std::left << std::setw(35) << item.filename.substr(0, 34)
                      << std::setw(12) << (item.isDirectory ? "[DIR]" : "[FILE]")
                      << std::setw(15) << item.humanSize << "\n";
        }
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << "[Test-Run Mode] Completed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "           FILE SYSTEM EXPLORER & METRICS INSPECTOR              \n";
    std::cout << "=================================================================\n";
    std::cout << "Current Directory: " << engine.getCurrentPath().string() << "\n";
    std::cout << "Run with --test-run for automated verification mode.\n";
    return 0;
}
