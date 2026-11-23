#include "StudentDatabase.h"

#include <sqlite3.h>
#include <iostream>

namespace student_db {

StudentDatabase::StudentDatabase() = default;

StudentDatabase::~StudentDatabase() {
    close();
}

StudentDatabase::StudentDatabase(StudentDatabase&& other) noexcept
    : m_db(other.m_db) {
    other.m_db = nullptr;
}

StudentDatabase& StudentDatabase::operator=(StudentDatabase&& other) noexcept {
    if (this != &other) {
        close();
        m_db = other.m_db;
        other.m_db = nullptr;
    }
    return *this;
}

bool StudentDatabase::open(const std::string& dbPath) {
    close();
    int rc = sqlite3_open(dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        close();
        return false;
    }
    return initializeSchema();
}

void StudentDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool StudentDatabase::initializeSchema() {
    if (!m_db) return false;

    const char* schemaSql = R"(
        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id TEXT UNIQUE NOT NULL,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            email TEXT NOT NULL,
            major TEXT NOT NULL,
            gpa REAL DEFAULT 0.0,
            credits_completed INTEGER DEFAULT 0,
            enrollment_date TEXT DEFAULT CURRENT_TIMESTAMP
        );
        CREATE INDEX IF NOT EXISTS idx_students_major ON students(major);
        CREATE INDEX IF NOT EXISTS idx_students_name ON students(last_name, first_name);
    )";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, schemaSql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool StudentDatabase::addStudent(const Student& s) {
    if (!m_db || s.studentId.empty() || s.firstName.empty() || s.lastName.empty()) {
        return false;
    }

    const char* sql = "INSERT INTO students (student_id, first_name, last_name, email, major, gpa, credits_completed, enrollment_date) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, s.studentId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.firstName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.lastName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, s.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, s.major.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 6, s.gpa);
    sqlite3_bind_int(stmt, 7, s.creditsCompleted);
    sqlite3_bind_text(stmt, 8, s.enrollmentDate.empty() ? "2026-09-01" : s.enrollmentDate.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::optional<Student> StudentDatabase::getStudentByStudentId(const std::string& studentId) const {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, student_id, first_name, last_name, email, major, gpa, credits_completed, enrollment_date "
                      "FROM students WHERE student_id = ? LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, studentId.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Student> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Student s;
        s.id = sqlite3_column_int(stmt, 0);
        s.studentId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s.firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        s.lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        s.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        s.major = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        s.gpa = sqlite3_column_double(stmt, 6);
        s.creditsCompleted = sqlite3_column_int(stmt, 7);
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        s.enrollmentDate = date ? date : "";
        result = s;
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<Student> StudentDatabase::getAllStudents() const {
    if (!m_db) return {};

    const char* sql = "SELECT id, student_id, first_name, last_name, email, major, gpa, credits_completed, enrollment_date "
                      "FROM students ORDER BY last_name, first_name;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::vector<Student> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Student s;
        s.id = sqlite3_column_int(stmt, 0);
        s.studentId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s.firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        s.lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        s.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        s.major = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        s.gpa = sqlite3_column_double(stmt, 6);
        s.creditsCompleted = sqlite3_column_int(stmt, 7);
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        s.enrollmentDate = date ? date : "";
        results.push_back(s);
    }

    sqlite3_finalize(stmt);
    return results;
}

std::vector<Student> StudentDatabase::searchStudents(const std::string& searchTerm) const {
    if (!m_db) return {};

    const char* sql = "SELECT id, student_id, first_name, last_name, email, major, gpa, credits_completed, enrollment_date "
                      "FROM students WHERE first_name LIKE ? OR last_name LIKE ? OR major LIKE ? OR student_id LIKE ? "
                      "ORDER BY last_name, first_name;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::string wildcard = "%" + searchTerm + "%";
    for (int i = 1; i <= 4; ++i) {
        sqlite3_bind_text(stmt, i, wildcard.c_str(), -1, SQLITE_TRANSIENT);
    }

    std::vector<Student> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Student s;
        s.id = sqlite3_column_int(stmt, 0);
        s.studentId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s.firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        s.lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        s.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        s.major = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        s.gpa = sqlite3_column_double(stmt, 6);
        s.creditsCompleted = sqlite3_column_int(stmt, 7);
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        s.enrollmentDate = date ? date : "";
        results.push_back(s);
    }

    sqlite3_finalize(stmt);
    return results;
}

bool StudentDatabase::updateGpa(const std::string& studentId, double newGpa) {
    if (!m_db || newGpa < 0.0 || newGpa > 4.0) return false;

    const char* sql = "UPDATE students SET gpa = ? WHERE student_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_double(stmt, 1, newGpa);
    sqlite3_bind_text(stmt, 2, studentId.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE && changes > 0);
}

bool StudentDatabase::deleteStudent(const std::string& studentId) {
    if (!m_db) return false;

    const char* sql = "DELETE FROM students WHERE student_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, studentId.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE && changes > 0);
}

AcademicStats StudentDatabase::getAcademicStats() const {
    AcademicStats stats;
    if (!m_db) return stats;

    const char* sql = "SELECT COUNT(*), AVG(gpa), MAX(gpa), MIN(gpa), SUM(credits_completed) FROM students;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats.totalStudents = static_cast<size_t>(sqlite3_column_int64(stmt, 0));
            stats.averageGpa = sqlite3_column_double(stmt, 1);
            stats.highestGpa = sqlite3_column_double(stmt, 2);
            stats.lowestGpa = sqlite3_column_double(stmt, 3);
            stats.totalCredits = sqlite3_column_int(stmt, 4);
        }
        sqlite3_finalize(stmt);
    }
    return stats;
}

bool StudentDatabase::beginTransaction() {
    return sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool StudentDatabase::commitTransaction() {
    return sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool StudentDatabase::rollbackTransaction() {
    return sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr) == SQLITE_OK;
}

size_t StudentDatabase::count() const {
    if (!m_db) return 0;
    const char* sql = "SELECT COUNT(*) FROM students;";
    sqlite3_stmt* stmt = nullptr;
    size_t c = 0;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            c = static_cast<size_t>(sqlite3_column_int64(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return c;
}

} // namespace student_db
