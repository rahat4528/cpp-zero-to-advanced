#include "StudentDatabase.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

void printHelp() {
    std::cout << "Student Record Database CLI\n"
              << "Usage:\n"
              << "  StudentRecordDatabase --list\n"
              << "  StudentRecordDatabase --add \"ID|First|Last|Email|Major|GPA|Credits\"\n"
              << "  StudentRecordDatabase --search <term>\n"
              << "  StudentRecordDatabase --update-gpa <studentId> <newGpa>\n"
              << "  StudentRecordDatabase --delete <studentId>\n"
              << "  StudentRecordDatabase --stats\n"
              << "  StudentRecordDatabase --db <sqlite_path>\n"
              << "  StudentRecordDatabase --help\n"
              << "Example:\n"
              << "  StudentRecordDatabase --add \"STU101|Grace|Hopper|grace@navy.mil|Computer Science|3.95|120\"\n";
}

void printTable(const std::vector<student_db::Student>& students) {
    if (students.empty()) {
        std::cout << "No student records found in database.\n";
        return;
    }
    std::cout << "\n====================================================================================================\n";
    std::cout << std::left << std::setw(10) << "STUDENT ID"
              << std::setw(22) << "NAME"
              << std::setw(24) << "EMAIL"
              << std::setw(20) << "MAJOR"
              << std::setw(8)  << "GPA"
              << "CREDITS\n";
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    for (const auto& s : students) {
        std::string fullName = s.lastName + ", " + s.firstName;
        std::cout << std::left << std::setw(10) << s.studentId
                  << std::setw(22) << fullName
                  << std::setw(24) << s.email
                  << std::setw(20) << s.major
                  << std::fixed << std::setprecision(2) << std::setw(8) << s.gpa
                  << s.creditsCompleted << "\n";
    }
    std::cout << "====================================================================================================\n";
}

int main(int argc, char* argv[]) {
    std::string dbPath = "students.db";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--db" && i + 1 < argc) {
            dbPath = argv[++i];
        }
    }

    student_db::StudentDatabase db;
    if (!db.open(dbPath)) {
        std::cerr << "Error: Unable to open SQLite database at '" << dbPath << "'.\n";
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--list") {
            printTable(db.getAllStudents());
            return 0;
        } else if (arg == "--search" && i + 1 < argc) {
            std::string q = argv[++i];
            printTable(db.searchStudents(q));
            return 0;
        } else if (arg == "--stats") {
            auto stats = db.getAcademicStats();
            std::cout << "\n========================================\n"
                      << "       ACADEMIC DATABASE METRICS        \n"
                      << "========================================\n"
                      << "Total Enrolled Students : " << stats.totalStudents << "\n"
                      << std::fixed << std::setprecision(2)
                      << "Average Cumulative GPA  : " << stats.averageGpa << "\n"
                      << "Highest Student GPA     : " << stats.highestGpa << "\n"
                      << "Lowest Student GPA      : " << stats.lowestGpa << "\n"
                      << "Total Completed Credits : " << stats.totalCredits << "\n"
                      << "========================================\n";
            return 0;
        } else if (arg == "--delete" && i + 1 < argc) {
            std::string sid = argv[++i];
            if (db.deleteStudent(sid)) {
                std::cout << "Student " << sid << " deleted successfully.\n";
            } else {
                std::cerr << "Student " << sid << " not found.\n";
                return 1;
            }
            return 0;
        } else if (arg == "--update-gpa" && i + 2 < argc) {
            std::string sid = argv[++i];
            double gpa = std::stod(argv[++i]);
            if (db.updateGpa(sid, gpa)) {
                std::cout << "Updated GPA for " << sid << " to " << gpa << ".\n";
            } else {
                std::cerr << "Failed to update GPA for " << sid << ".\n";
                return 1;
            }
            return 0;
        } else if (arg == "--add" && i + 1 < argc) {
            std::string payload = argv[++i];
            std::stringstream ss(payload);
            student_db::Student s;
            std::string gpaStr, credStr;

            std::getline(ss, s.studentId, '|');
            std::getline(ss, s.firstName, '|');
            std::getline(ss, s.lastName, '|');
            std::getline(ss, s.email, '|');
            std::getline(ss, s.major, '|');
            std::getline(ss, gpaStr, '|');
            std::getline(ss, credStr);

            if (!gpaStr.empty()) s.gpa = std::stod(gpaStr);
            if (!credStr.empty()) s.creditsCompleted = std::stoi(credStr);

            if (db.addStudent(s)) {
                std::cout << "Student " << s.studentId << " (" << s.firstName << " " << s.lastName << ") enrolled successfully.\n";
            } else {
                std::cerr << "Error adding student (duplicate ID or invalid fields).\n";
                return 1;
            }
            return 0;
        }
    }

    std::cout << "=== Student Record Database CLI ===\n";
    printTable(db.getAllStudents());
    return 0;
}
