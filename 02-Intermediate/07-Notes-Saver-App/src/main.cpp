#include "NotesDatabase.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string dbFile = "notes.db";

    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
            dbFile = ":memory:";
        } else if (std::string(argv[i]) == "--db" && i + 1 < argc) {
            dbFile = argv[++i];
        }
    }

    notes::NotesDatabase db(dbFile);
    if (!db.open()) {
        std::cerr << "Failed to open notes database.\n";
        return 1;
    }

    if (testRun) {
        std::cout << "[Test-Run Mode] Creating test notes, toggling pins, and running full-text search...\n";
        int id1 = db.createNote("Modern C++ Concurrency", "Understanding std::jthread, mutexes, and atomics.", "cpp,threads,modern", true);
        int id2 = db.createNote("Grocery Shopping List", "- 2L Milk\n- Whole wheat bread\n- Green tea", "personal,shopping", false);
        int id3 = db.createNote("CMake Best Practices", "Prefer target_include_directories and target_link_libraries.", "cmake,build,cpp", false);

        std::cout << "Created notes with IDs: " << id1 << ", " << id2 << ", " << id3 << "\n";

        auto all = db.getAllNotes();
        std::cout << "Total Notes: " << all.size() << "\n";
        for (const auto& n : all) {
            std::cout << " [" << n.id << "] " << (n.isPinned ? "[PINNED] " : "") << n.title
                      << " (Tags: " << n.tags << ")\n";
        }

        auto searchRes = db.searchNotes("concurrency");
        std::cout << "Search 'concurrency' found: " << searchRes.size() << " match(es)\n";

        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "                   MARKDOWN NOTES SAVER (SQLite)                 \n";
    std::cout << "=================================================================\n";

    auto notesList = db.getAllNotes();
    std::cout << "Total Saved Notes: " << notesList.size() << "\n";
    for (const auto& n : notesList) {
        std::cout << " [" << n.id << "] " << (n.isPinned ? "[*PINNED*] " : "") << n.title << "\n";
        std::cout << "     Tags: " << n.tags << " | Updated: " << n.updatedAt << "\n";
    }

    return 0;
}
