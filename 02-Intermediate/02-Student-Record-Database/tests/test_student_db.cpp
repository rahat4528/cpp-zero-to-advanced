#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "StudentDatabase.h"

using namespace student_db;

TEST_CASE("In-memory SQLite database open and schema creation") {
    StudentDatabase db;
    CHECK(db.open(":memory:") == true);
    CHECK(db.isOpen() == true);
    CHECK(db.count() == 0);
}

TEST_CASE("CRUD Operations with Parameterized Prepared Statements") {
    StudentDatabase db;
    REQUIRE(db.open(":memory:") == true);

    Student s1;
    s1.studentId = "STU101";
    s1.firstName = "Alan";
    s1.lastName = "Turing";
    s1.email = "alan@cambridge.ac.uk";
    s1.major = "Mathematics";
    s1.gpa = 3.98;
    s1.creditsCompleted = 120;

    // Add student
    CHECK(db.addStudent(s1) == true);
    CHECK(db.count() == 1);

    // Duplicate studentId rejection
    CHECK(db.addStudent(s1) == false);

    // Query student
    auto found = db.getStudentByStudentId("STU101");
    REQUIRE(found.has_value());
    CHECK(found->firstName == "Alan");
    CHECK(found->gpa == doctest::Approx(3.98));

    // Update GPA
    CHECK(db.updateGpa("STU101", 4.0) == true);
    auto updated = db.getStudentByStudentId("STU101");
    REQUIRE(updated.has_value());
    CHECK(updated->gpa == doctest::Approx(4.0));

    // Delete student
    CHECK(db.deleteStudent("STU101") == true);
    CHECK(db.count() == 0);
    CHECK(db.getStudentByStudentId("STU101").has_value() == false);
}

TEST_CASE("SQL Injection attack prevention via prepared statements") {
    StudentDatabase db;
    REQUIRE(db.open(":memory:") == true);

    // Attempt SQL injection in name and studentId
    Student malicious;
    malicious.studentId = "STU'; DROP TABLE students; --";
    malicious.firstName = "Robert'); DROP TABLE students;--";
    malicious.lastName = "Tables";
    malicious.email = "bobby@xkcd.com";
    malicious.major = "CS";
    malicious.gpa = 3.5;

    // Parameterized statement must safely store it without executing the injection
    CHECK(db.addStudent(malicious) == true);
    CHECK(db.count() == 1);

    // The table must still exist and be readable
    auto found = db.getStudentByStudentId("STU'; DROP TABLE students; --");
    REQUIRE(found.has_value());
    CHECK(found->lastName == "Tables");
}

TEST_CASE("Search and academic statistics") {
    StudentDatabase db;
    REQUIRE(db.open(":memory:") == true);

    db.addStudent({0, "S1", "Ada", "Lovelace", "ada@analytics.org", "Computer Science", 3.9, 90, ""});
    db.addStudent({0, "S2", "Charles", "Babbage", "charles@engines.org", "Mechanical Engineering", 3.5, 80, ""});
    db.addStudent({0, "S3", "Claude", "Shannon", "claude@bell.com", "Computer Science", 3.8, 85, ""});

    auto csStudents = db.searchStudents("Computer Science");
    CHECK(csStudents.size() == 2);

    auto stats = db.getAcademicStats();
    CHECK(stats.totalStudents == 3);
    CHECK(stats.highestGpa == doctest::Approx(3.9));
    CHECK(stats.lowestGpa == doctest::Approx(3.5));
    CHECK(stats.averageGpa == doctest::Approx(3.7333).epsilon(0.01));
}

TEST_CASE("Transaction rollback on failure") {
    StudentDatabase db;
    REQUIRE(db.open(":memory:") == true);

    CHECK(db.beginTransaction() == true);
    db.addStudent({0, "T1", "Test", "User", "test@test.com", "CS", 3.0, 30, ""});
    CHECK(db.count() == 1);
    CHECK(db.rollbackTransaction() == true);

    // After rollback, student count must be 0
    CHECK(db.count() == 0);
}
