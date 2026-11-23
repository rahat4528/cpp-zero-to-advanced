#include "NotesDatabase.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

namespace notes {

NotesDatabase::NotesDatabase(const std::string& dbPath) : m_dbPath(dbPath) {}

NotesDatabase::~NotesDatabase() {
    close();
}

bool NotesDatabase::open() {
    if (m_db) return true;
    if (sqlite3_open(m_dbPath.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "SQLite open error: " << sqlite3_errmsg(m_db) << "\n";
        m_db = nullptr;
        return false;
    }
    initSchema();
    return true;
}

void NotesDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

void NotesDatabase::initSchema() {
    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS notes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            content TEXT NOT NULL,
            tags TEXT NOT NULL,
            is_pinned INTEGER NOT NULL DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        CREATE INDEX IF NOT EXISTS idx_notes_pinned ON notes(is_pinned DESC, updated_at DESC);
    )";

    char* err = nullptr;
    if (sqlite3_exec(m_db, schema, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string errStr = err ? err : "Unknown error";
        sqlite3_free(err);
        throw std::runtime_error("Failed to initialize notes schema: " + errStr);
    }
}

int NotesDatabase::createNote(const std::string& title, const std::string& content, const std::string& tags, bool isPinned) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO notes (title, content, tags, is_pinned, created_at, updated_at) "
                      "VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, tags.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, isPinned ? 1 : 0);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        id = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    }
    sqlite3_finalize(stmt);
    return id;
}

bool NotesDatabase::updateNote(int id, const std::string& title, const std::string& content, const std::string& tags, bool isPinned) {
    if (!m_db) return false;

    const char* sql = "UPDATE notes SET title = ?, content = ?, tags = ?, is_pinned = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, tags.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, isPinned ? 1 : 0);
    sqlite3_bind_int(stmt, 5, id);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);
    return ok && (changes > 0);
}

bool NotesDatabase::deleteNote(int id) {
    if (!m_db) return false;

    const char* sql = "DELETE FROM notes WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);
    return ok && (changes > 0);
}

std::optional<Note> NotesDatabase::getNote(int id) {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, title, content, tags, is_pinned, created_at, updated_at FROM notes WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Note n;
        n.id = sqlite3_column_int(stmt, 0);
        n.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        n.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        n.tags = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        n.isPinned = (sqlite3_column_int(stmt, 4) != 0);
        n.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        n.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

        sqlite3_finalize(stmt);
        return n;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<Note> NotesDatabase::getAllNotes() {
    std::vector<Note> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, title, content, tags, is_pinned, created_at, updated_at "
                      "FROM notes ORDER BY is_pinned DESC, updated_at DESC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Note n;
        n.id = sqlite3_column_int(stmt, 0);
        n.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        n.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        n.tags = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        n.isPinned = (sqlite3_column_int(stmt, 4) != 0);
        n.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        n.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        list.push_back(n);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<Note> NotesDatabase::searchNotes(const std::string& keyword) {
    std::vector<Note> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, title, content, tags, is_pinned, created_at, updated_at "
                      "FROM notes WHERE title LIKE ? OR content LIKE ? "
                      "ORDER BY is_pinned DESC, updated_at DESC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    std::string pattern = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Note n;
        n.id = sqlite3_column_int(stmt, 0);
        n.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        n.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        n.tags = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        n.isPinned = (sqlite3_column_int(stmt, 4) != 0);
        n.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        n.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        list.push_back(n);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<Note> NotesDatabase::getNotesByTag(const std::string& tag) {
    std::vector<Note> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, title, content, tags, is_pinned, created_at, updated_at "
                      "FROM notes WHERE (',' || tags || ',') LIKE ? "
                      "ORDER BY is_pinned DESC, updated_at DESC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    std::string pattern = "%," + tag + ",%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Note n;
        n.id = sqlite3_column_int(stmt, 0);
        n.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        n.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        n.tags = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        n.isPinned = (sqlite3_column_int(stmt, 4) != 0);
        n.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        n.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        list.push_back(n);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool NotesDatabase::togglePin(int id) {
    auto n = getNote(id);
    if (!n) return false;
    return updateNote(id, n->title, n->content, n->tags, !n->isPinned);
}

bool NotesDatabase::exportToMarkdown(int id, const std::string& outputPath) {
    auto n = getNote(id);
    if (!n) return false;

    std::ofstream out(outputPath);
    if (!out.is_open()) return false;

    out << "# " << n->title << "\n\n";
    out << "> **Tags:** `" << n->tags << "`  \n";
    out << "> **Created:** " << n->createdAt << " | **Updated:** " << n->updatedAt << "\n\n";
    out << "---\n\n";
    out << n->content << "\n";
    return true;
}

} // namespace notes
