#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>

namespace notes {

struct Note {
    int id{0};
    std::string title;
    std::string content;
    std::string tags; // Comma-delimited: "cpp,study,todo"
    bool isPinned{false};
    std::string createdAt;
    std::string updatedAt;
};

class NotesDatabase {
public:
    explicit NotesDatabase(const std::string& dbPath);
    ~NotesDatabase();

    bool open();
    void close();

    int createNote(const std::string& title, const std::string& content, const std::string& tags, bool isPinned = false);
    bool updateNote(int id, const std::string& title, const std::string& content, const std::string& tags, bool isPinned);
    bool deleteNote(int id);
    std::optional<Note> getNote(int id);

    std::vector<Note> getAllNotes();
    std::vector<Note> searchNotes(const std::string& keyword);
    std::vector<Note> getNotesByTag(const std::string& tag);
    bool togglePin(int id);

    bool exportToMarkdown(int id, const std::string& outputPath);

private:
    void initSchema();

    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace notes
