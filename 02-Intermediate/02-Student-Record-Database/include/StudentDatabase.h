#pragma once

#include <optional>
#include <string>
#include <vector>

struct sqlite3;

namespace student_db {

struct Student {
    int id{0};
    std::string studentId; // e.g. "STU1001"
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string major;
    double gpa{0.0};
    int creditsCompleted{0};
    std::string enrollmentDate;
};

struct AcademicStats {
    size_t totalStudents{0};
    double averageGpa{0.0};
    double highestGpa{0.0};
    double lowestGpa{0.0};
    int totalCredits{0};
};

class StudentDatabase {
public:
    StudentDatabase();
    ~StudentDatabase();

    // Prevent copy, allow move (RAII SQLite handle)
    StudentDatabase(const StudentDatabase&) = delete;
    StudentDatabase& operator=(const StudentDatabase&) = delete;
    StudentDatabase(StudentDatabase&& other) noexcept;
    StudentDatabase& operator=(StudentDatabase&& other) noexcept;

    // Open database (file path or ":memory:")
    bool open(const std::string& dbPath);
    void close();
    bool isOpen() const { return m_db != nullptr; }

    // Schema Initialization
    bool initializeSchema();

    // CRUD Operations (All using parameterized prepared statements)
    bool addStudent(const Student& student);
    std::optional<Student> getStudentByStudentId(const std::string& studentId) const;
    std::vector<Student> getAllStudents() const;
    std::vector<Student> searchStudents(const std::string& searchTerm) const;
    bool updateGpa(const std::string& studentId, double newGpa);
    bool deleteStudent(const std::string& studentId);

    // Analytics
    AcademicStats getAcademicStats() const;

    // Transaction Management
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    size_t count() const;

private:
    sqlite3* m_db{nullptr};
};

} // namespace student_db
