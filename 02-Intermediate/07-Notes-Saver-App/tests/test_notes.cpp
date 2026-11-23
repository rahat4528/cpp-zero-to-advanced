#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "NotesDatabase.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("NotesDatabase - CRUD, Searching, Pinning, and Export") {
    notes::NotesDatabase db(":memory:");
    REQUIRE(db.open() == true);

    SUBCASE("Create and Retrieve Note") {
        int id = db.createNote("Design Patterns", "Singleton, Factory, Observer in C++", "cpp,patterns", false);
        REQUIRE(id > 0);

        auto note = db.getNote(id);
        REQUIRE(note.has_value());
        CHECK(note->title == "Design Patterns");
        CHECK(note->content == "Singleton, Factory, Observer in C++");
        CHECK(note->isPinned == false);
    }

    SUBCASE("Update and Pin Toggle") {
        int id = db.createNote("Old Title", "Old Content", "tag1", false);
        CHECK(db.updateNote(id, "New Title", "New Content", "tag1,tag2", true) == true);

        auto updated = db.getNote(id);
        REQUIRE(updated.has_value());
        CHECK(updated->title == "New Title");
        CHECK(updated->isPinned == true);

        CHECK(db.togglePin(id) == true);
        auto unpinned = db.getNote(id);
        REQUIRE(unpinned.has_value());
        CHECK(unpinned->isPinned == false);
    }

    SUBCASE("Tag and Keyword Searching") {
        db.createNote("RAII Guide", "Resource Acquisition Is Initialization", "cpp,best-practice", false);
        db.createNote("Python Tips", "List comprehensions and generators", "python,scripting", false);
        db.createNote("Database Schema", "SQLite indexes and ACID properties", "sql,database,cpp", true);

        auto cppNotes = db.getNotesByTag("cpp");
        CHECK(cppNotes.size() == 2);

        auto searchRes = db.searchNotes("Initialization");
        REQUIRE(searchRes.size() == 1);
        CHECK(searchRes[0].title == "RAII Guide");
    }

    SUBCASE("Delete Note") {
        int id = db.createNote("To Delete", "Delete me soon", "temp", false);
        CHECK(db.deleteNote(id) == true);
        CHECK(db.getNote(id).has_value() == false);
    }

    SUBCASE("Export to Markdown File") {
        int id = db.createNote("Export Note", "Sample markdown body content.", "test,export", true);
        std::string filename = "temp_export_test.md";
        CHECK(db.exportToMarkdown(id, filename) == true);

        CHECK(std::filesystem::exists(filename));
        std::filesystem::remove(filename);
    }
}
