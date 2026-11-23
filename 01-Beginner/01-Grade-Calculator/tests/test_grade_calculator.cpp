#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "GradeCalculator.h"

using namespace grade_calculator;

TEST_CASE("Grade Table Boundaries: 39.99, 40, 59.99, 60") {
    GradeTable table = GradeTable::createDefault();

    // Below 40 -> F (0.0)
    auto [l1, p1] = letter_for(39.99, table);
    CHECK(l1 == "F");
    CHECK(p1 == doctest::Approx(0.0));

    // Exactly 40 -> D (2.0)
    auto [l2, p2] = letter_for(40.0, table);
    CHECK(l2 == "D");
    CHECK(p2 == doctest::Approx(2.0));

    // 59.99 -> B- (2.75) since 55 <= 59.99 < 60
    auto [l3, p3] = letter_for(59.99, table);
    CHECK(l3 == "B-");
    CHECK(p3 == doctest::Approx(2.75));

    // Exactly 60 -> B (3.0)
    auto [l4, p4] = letter_for(60.0, table);
    CHECK(l4 == "B");
    CHECK(p4 == doctest::Approx(3.0));
}

TEST_CASE("Extremes: 0% and 100%") {
    GradeTable table = GradeTable::createDefault();

    auto [lZero, pZero] = letter_for(0.0, table);
    CHECK(lZero == "F");
    CHECK(pZero == doctest::Approx(0.0));

    auto [lHundred, pHundred] = letter_for(100.0, table);
    CHECK(lHundred == "A+");
    CHECK(pHundred == doctest::Approx(4.0));
}

TEST_CASE("Custom Max Marks: max = 50") {
    CHECK(percentage(25.0, 50.0) == doctest::Approx(50.0));
    CHECK(percentage(50.0, 50.0) == doctest::Approx(100.0));
    CHECK(percentage(0.0, 50.0) == doctest::Approx(0.0));
    CHECK(percentage(20.0, 50.0) == doctest::Approx(40.0));

    auto [letter, pt] = letter_for(percentage(20.0, 50.0));
    CHECK(letter == "D");
    CHECK(pt == doctest::Approx(2.0));
}

TEST_CASE("Single Subject Report") {
    std::vector<Subject> subs = {
        {"Physics", 85.0, 100.0, 3.0}
    };
    auto report = generateReport("John Doe", subs);
    CHECK(report.totalMarksObtained == doctest::Approx(85.0));
    CHECK(report.totalMaxMarks == doctest::Approx(100.0));
    CHECK(report.overallPercentage == doctest::Approx(85.0));
    CHECK(report.weightedGpa == doctest::Approx(4.0)); // 85% -> A+ (4.0)
    CHECK(report.overallResult == "PASS");
}

TEST_CASE("All-Fail Condition and Partial Fail Condition") {
    std::vector<Subject> allFail = {
        {"Sub1", 20.0, 100.0, 1.0},
        {"Sub2", 30.0, 100.0, 1.0}
    };
    CHECK(overall_result(allFail) == "FAIL");

    // One subject failed (<40%), even if average is high (e.g. 90 and 35 -> avg 62.5% B)
    std::vector<Subject> mixed = {
        {"SubA", 95.0, 100.0, 1.0},
        {"SubB", 38.0, 100.0, 1.0}
    };
    auto rep = generateReport("Jane", mixed);
    CHECK(rep.overallPercentage == doctest::Approx(66.5));
    CHECK(rep.overallResult == "FAIL"); // Must fail if any subject < 40%
}

TEST_CASE("Hand-Computed Weighted GPA") {
    // Sub1: 82% -> A+ (4.0), credit 4.0 -> weighted: 16.0
    // Sub2: 76% -> A  (3.75), credit 2.0 -> weighted: 7.5
    // Sub3: 62% -> B  (3.0), credit 2.0 -> weighted: 6.0
    // Total weighted = 29.5, Total credits = 8.0 -> GPA = 29.5 / 8.0 = 3.6875
    std::vector<Subject> subs = {
        {"Sub1", 82.0, 100.0, 4.0},
        {"Sub2", 76.0, 100.0, 2.0},
        {"Sub3", 62.0, 100.0, 2.0}
    };
    double calculatedGpa = gpa(subs);
    CHECK(calculatedGpa == doctest::Approx(3.6875));
    CHECK(overall_result(subs) == "PASS");
}

TEST_CASE("Validation: Input Rejection and Boundary Errors") {
    CHECK(isValidMarks(50.0, 100.0) == true);
    CHECK(isValidMarks(0.0, 100.0) == true);
    CHECK(isValidMarks(100.0, 100.0) == true);

    // Marks > maxMarks rejected
    CHECK(isValidMarks(105.0, 100.0) == false);
    CHECK(isValidMarks(-1.0, 100.0) == false);
    CHECK(isValidMarks(50.0, 0.0) == false);
    CHECK(isValidMarks(50.0, -10.0) == false);

    // Subject count validation (1 to 20)
    CHECK(isValidSubjectCount(0) == false);
    CHECK(isValidSubjectCount(1) == true);
    CHECK(isValidSubjectCount(20) == true);
    CHECK(isValidSubjectCount(21) == false);
    CHECK(isValidSubjectCount(-5) == false);

    // Percentage throws on invalid input
    CHECK_THROWS_AS(percentage(105.0, 100.0), std::invalid_argument);
    CHECK_THROWS_AS(percentage(-5.0, 100.0), std::invalid_argument);
    CHECK_THROWS_AS(percentage(50.0, 0.0), std::invalid_argument);
}
