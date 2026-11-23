#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "FileExplorer.hpp"
#include <fstream>

TEST_CASE("FileExplorerEngine - Formatting and Calculations") {
    SUBCASE("Human Readable Byte Formatting") {
        CHECK(explorer::FileExplorerEngine::formatBytes(512) == "512.0 B");
        CHECK(explorer::FileExplorerEngine::formatBytes(1024) == "1.0 KB");
        CHECK(explorer::FileExplorerEngine::formatBytes(1024 * 1024 * 5) == "5.0 MB");
    }
}

TEST_CASE("FileExplorerEngine - Directory Traversal and Sorting") {
    std::filesystem::path testDir = "temp_explorer_test_dir";
    std::filesystem::create_directories(testDir / "subfolder");

    std::ofstream f1(testDir / "b_file.txt");
    f1 << "12345"; // 5 bytes
    f1.close();

    std::ofstream f2(testDir / "a_file.cpp");
    f2 << "1234567890"; // 10 bytes
    f2.close();

    explorer::FileExplorerEngine engine(testDir);
    CHECK(engine.getCurrentPath() == testDir);

    SUBCASE("List All Entries (Directories First)") {
        auto entries = engine.listEntries("", explorer::SortCriteria::NameAsc);
        REQUIRE(entries.size() == 3);
        CHECK(entries[0].isDirectory == true);
        CHECK(entries[0].filename == "subfolder");
        CHECK(entries[1].filename == "a_file.cpp");
        CHECK(entries[2].filename == "b_file.txt");
    }

    SUBCASE("Filter By Extension") {
        auto cppOnly = engine.listEntries(".cpp");
        REQUIRE(cppOnly.size() == 2); // Includes navigable directory + matching .cpp file
        CHECK(cppOnly[0].isDirectory == true);
        CHECK(cppOnly[1].filename == "a_file.cpp");
    }

    SUBCASE("Sort By Size Descending") {
        auto sortedSize = engine.listEntries("", explorer::SortCriteria::SizeDesc);
        REQUIRE(sortedSize.size() == 3);
        CHECK(sortedSize[0].isDirectory == true); // Dir first
        CHECK(sortedSize[1].sizeBytes == 10);
        CHECK(sortedSize[2].sizeBytes == 5);
    }

    SUBCASE("Recursive Size Calculation") {
        uintmax_t size = explorer::FileExplorerEngine::calculateRecursiveDirectorySize(testDir);
        CHECK(size == 15);
    }

    std::filesystem::remove_all(testDir);
}
