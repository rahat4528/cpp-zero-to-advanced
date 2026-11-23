#pragma once

#include <string>
#include <vector>
#include <utility>

namespace grade_calculator {

struct Subject {
    std::string name;
    double marksObtained{0.0};
    double maxMarks{100.0};
    double credit{1.0};
};

struct GradeRule {
    double minPercentage{0.0};
    std::string letter;
    double gradePoint{0.0};
};

struct GradeTable {
    std::vector<GradeRule> rules;

    static GradeTable createDefault();
};

struct SubjectResult {
    Subject subject;
    double percentage{0.0};
    std::string letterGrade;
    double gradePoint{0.0};
};

struct StudentReport {
    std::string studentName;
    std::vector<SubjectResult> subjects;
    double totalMarksObtained{0.0};
    double totalMaxMarks{0.0};
    double overallPercentage{0.0};
    double weightedGpa{0.0};
    std::string overallResult; // "PASS" or "FAIL"
};

// Pure functions per prompt DESIGN specification
double percentage(double marks, double maxMarks);
std::pair<std::string, double> letter_for(double pct, const GradeTable& table = GradeTable::createDefault());
double gpa(const std::vector<Subject>& subjects, const GradeTable& table = GradeTable::createDefault());
std::string overall_result(const std::vector<Subject>& subjects, const GradeTable& table = GradeTable::createDefault());

// Report generation & formatting
StudentReport generateReport(const std::string& studentName,
                             const std::vector<Subject>& subjects,
                             const GradeTable& table = GradeTable::createDefault());
std::string formatReportTable(const StudentReport& report);

// Validation
bool isValidMarks(double marks, double maxMarks);
bool isValidSubjectCount(int count);

} // namespace grade_calculator
