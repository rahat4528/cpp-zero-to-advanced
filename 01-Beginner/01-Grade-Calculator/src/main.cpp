#include "GradeCalculator.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace {

void clearCin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string sanitizeFilename(const std::string& name) {
    std::string safe = name;
    for (char& c : safe) {
        if (c == ' ' || c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    return safe;
}

} // namespace

int main(int argc, char* argv[]) {
    // Non-interactive automated smoke test flag
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            std::vector<grade_calculator::Subject> testSubs = {
                {"Mathematics", 88.0, 100.0, 4.0},
                {"Physics", 74.0, 100.0, 3.0},
                {"Computer Science", 95.0, 100.0, 4.0}
            };
            auto rep = grade_calculator::generateReport("Alex Mercer", testSubs);
            std::cout << grade_calculator::formatReportTable(rep);
            std::cout << "[Test Run] Grade Calculator executed successfully.\n";
            return 0;
        }
    }

    std::cout << "==========================================\n";
    std::cout << "        C++ Grade Calculator System       \n";
    std::cout << "==========================================\n";

    std::string studentName;
    std::cout << "Enter student name: ";
    if (!std::getline(std::cin, studentName) || studentName.empty()) {
        studentName = "Student";
    }

    int numSubjects = 0;
    while (true) {
        std::cout << "Enter number of subjects (1-20): ";
        if (std::cin >> numSubjects && grade_calculator::isValidSubjectCount(numSubjects)) {
            clearCin();
            break;
        }
        std::cout << "Invalid count. Please enter an integer between 1 and 20.\n";
        clearCin();
    }

    std::vector<grade_calculator::Subject> subjects;
    subjects.reserve(numSubjects);

    for (int i = 0; i < numSubjects; ++i) {
        grade_calculator::Subject sub;
        std::cout << "\nSubject #" << (i + 1) << " Details:\n";
        std::cout << "  Name: ";
        std::getline(std::cin, sub.name);
        if (sub.name.empty()) {
            sub.name = "Subject " + std::to_string(i + 1);
        }

        while (true) {
            std::cout << "  Max marks (default 100, > 0): ";
            std::string maxStr;
            std::getline(std::cin, maxStr);
            if (maxStr.empty()) {
                sub.maxMarks = 100.0;
                break;
            }
            try {
                size_t idx = 0;
                double val = std::stod(maxStr, &idx);
                if (idx == maxStr.size() && val > 0.0) {
                    sub.maxMarks = val;
                    break;
                }
            } catch (...) {}
            std::cout << "  Invalid max marks. Enter a positive number.\n";
        }

        while (true) {
            std::cout << "  Marks obtained (0 to " << sub.maxMarks << "): ";
            if (std::cin >> sub.marksObtained && grade_calculator::isValidMarks(sub.marksObtained, sub.maxMarks)) {
                clearCin();
                break;
            }
            std::cout << "  Invalid marks. Must be between 0 and " << sub.maxMarks << ".\n";
            clearCin();
        }

        while (true) {
            std::cout << "  Credit hours (default 1.0, > 0): ";
            std::string credStr;
            std::getline(std::cin, credStr);
            if (credStr.empty()) {
                sub.credit = 1.0;
                break;
            }
            try {
                size_t idx = 0;
                double val = std::stod(credStr, &idx);
                if (idx == credStr.size() && val > 0.0) {
                    sub.credit = val;
                    break;
                }
            } catch (...) {}
            std::cout << "  Invalid credit hours. Enter a positive number.\n";
        }

        subjects.push_back(sub);
    }

    auto report = grade_calculator::generateReport(studentName, subjects);
    std::string reportText = grade_calculator::formatReportTable(report);
    std::cout << "\n" << reportText;

    std::cout << "Would you like to save this report to file? (y/n): ";
    char choice = 'n';
    if (std::cin >> choice && (choice == 'y' || choice == 'Y')) {
        std::string filename = "report_" + sanitizeFilename(studentName) + ".txt";
        std::ofstream ofs(filename);
        if (ofs) {
            ofs << reportText;
            std::cout << "Report successfully saved to " << filename << "\n";
        } else {
            std::cerr << "Failed to open file for writing.\n";
        }
    }

    return 0;
}
