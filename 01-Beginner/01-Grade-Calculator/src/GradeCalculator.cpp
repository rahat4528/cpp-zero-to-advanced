#include "GradeCalculator.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace grade_calculator {

GradeTable GradeTable::createDefault() {
    GradeTable table;
    // Default grading scale per specification:
    // 80+ A+ 4.0, 75 A 3.75, 70 A- 3.5, 65 B+ 3.25, 60 B 3.0,
    // 55 B- 2.75, 50 C+ 2.5, 45 C 2.25, 40 D 2.0, below 40 F 0.0
    table.rules = {
        {80.0, "A+", 4.00},
        {75.0, "A",  3.75},
        {70.0, "A-", 3.50},
        {65.0, "B+", 3.25},
        {60.0, "B",  3.00},
        {55.0, "B-", 2.75},
        {50.0, "C+", 2.50},
        {45.0, "C",  2.25},
        {40.0, "D",  2.00},
        {0.0,  "F",  0.00}
    };
    return table;
}

bool isValidMarks(double marks, double maxMarks) {
    if (std::isnan(marks) || std::isnan(maxMarks)) return false;
    if (maxMarks <= 0.0) return false;
    if (marks < 0.0 || marks > maxMarks) return false;
    return true;
}

bool isValidSubjectCount(int count) {
    return count >= 1 && count <= 20;
}

double percentage(double marks, double maxMarks) {
    if (maxMarks <= 0.0) {
        throw std::invalid_argument("Maximum marks must be strictly positive");
    }
    if (marks < 0.0 || marks > maxMarks) {
        throw std::invalid_argument("Marks obtained must be within [0, maxMarks]");
    }
    return (marks / maxMarks) * 100.0;
}

std::pair<std::string, double> letter_for(double pct, const GradeTable& table) {
    for (const auto& rule : table.rules) {
        if (pct >= rule.minPercentage) {
            return {rule.letter, rule.gradePoint};
        }
    }
    return {"F", 0.0};
}

double gpa(const std::vector<Subject>& subjects, const GradeTable& table) {
    if (subjects.empty()) return 0.0;

    double totalWeightedPoints = 0.0;
    double totalCredits = 0.0;

    for (const auto& sub : subjects) {
        double pct = percentage(sub.marksObtained, sub.maxMarks);
        auto [letter, pt] = letter_for(pct, table);
        (void)letter;
        double cred = sub.credit > 0.0 ? sub.credit : 1.0;
        totalWeightedPoints += (pt * cred);
        totalCredits += cred;
    }

    if (totalCredits <= 0.0) return 0.0;
    return totalWeightedPoints / totalCredits;
}

std::string overall_result(const std::vector<Subject>& subjects, const GradeTable& table) {
    if (subjects.empty()) return "FAIL";
    for (const auto& sub : subjects) {
        double pct = percentage(sub.marksObtained, sub.maxMarks);
        auto [letter, pt] = letter_for(pct, table);
        (void)letter;
        (void)pt;
        // Overall result is FAIL if any subject is below 40%
        if (pct < 40.0) {
            return "FAIL";
        }
    }
    return "PASS";
}

StudentReport generateReport(const std::string& studentName,
                             const std::vector<Subject>& subjects,
                             const GradeTable& table) {
    StudentReport report;
    report.studentName = studentName;
    report.totalMarksObtained = 0.0;
    report.totalMaxMarks = 0.0;

    for (const auto& sub : subjects) {
        SubjectResult sr;
        sr.subject = sub;
        sr.percentage = percentage(sub.marksObtained, sub.maxMarks);
        auto [letter, pt] = letter_for(sr.percentage, table);
        sr.letterGrade = letter;
        sr.gradePoint = pt;

        report.subjects.push_back(sr);
        report.totalMarksObtained += sub.marksObtained;
        report.totalMaxMarks += sub.maxMarks;
    }

    if (report.totalMaxMarks > 0.0) {
        report.overallPercentage = (report.totalMarksObtained / report.totalMaxMarks) * 100.0;
    } else {
        report.overallPercentage = 0.0;
    }

    report.weightedGpa = gpa(subjects, table);
    report.overallResult = overall_result(subjects, table);
    return report;
}

std::string formatReportTable(const StudentReport& report) {
    std::ostringstream oss;
    oss << "=========================================================================\n";
    oss << "                        STUDENT GRADE REPORT                             \n";
    oss << "=========================================================================\n";
    oss << "Student Name: " << report.studentName << "\n";
    oss << "-------------------------------------------------------------------------\n";
    oss << std::left << std::setw(22) << "Subject"
        << std::right << std::setw(10) << "Marks"
        << std::setw(10) << "Max"
        << std::setw(10) << "Credit"
        << std::setw(10) << "Pct(%)"
        << std::setw(8)  << "Grade"
        << std::setw(8)  << "Point" << "\n";
    oss << "-------------------------------------------------------------------------\n";

    for (const auto& sr : report.subjects) {
        oss << std::left << std::setw(22) << sr.subject.name
            << std::right << std::fixed << std::setprecision(1)
            << std::setw(10) << sr.subject.marksObtained
            << std::setw(10) << sr.subject.maxMarks
            << std::setw(10) << sr.subject.credit
            << std::setprecision(2)
            << std::setw(10) << sr.percentage
            << std::setw(8)  << sr.letterGrade
            << std::setw(8)  << sr.gradePoint << "\n";
    }

    oss << "-------------------------------------------------------------------------\n";
    oss << std::fixed << std::setprecision(2);
    oss << "Total Marks Obtained : " << report.totalMarksObtained << " / " << report.totalMaxMarks << "\n";
    oss << "Overall Percentage   : " << report.overallPercentage << "%\n";
    oss << "Weighted GPA         : " << report.weightedGpa << " / 4.00\n";
    oss << "Final Result         : " << report.overallResult << "\n";
    oss << "=========================================================================\n";
    return oss.str();
}

} // namespace grade_calculator
