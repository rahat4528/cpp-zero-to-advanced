#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "CalendarReminder.h"

#include <cstdio>

using namespace calendar_reminder;

TEST_CASE("Leap year calculations") {
    CHECK(CalendarHelper::isLeapYear(2020) == true);
    CHECK(CalendarHelper::isLeapYear(2024) == true);
    CHECK(CalendarHelper::isLeapYear(2000) == true);
    CHECK(CalendarHelper::isLeapYear(1900) == false); // Century non-leap
    CHECK(CalendarHelper::isLeapYear(2021) == false);
    CHECK(CalendarHelper::isLeapYear(2026) == false);
}

TEST_CASE("Days in month") {
    CHECK(CalendarHelper::getDaysInMonth(2024, 2) == 29); // Leap Feb
    CHECK(CalendarHelper::getDaysInMonth(2026, 2) == 28); // Normal Feb
    CHECK(CalendarHelper::getDaysInMonth(2026, 4) == 30);
    CHECK(CalendarHelper::getDaysInMonth(2026, 7) == 31);
    CHECK(CalendarHelper::getDaysInMonth(2026, 11) == 30);
}

TEST_CASE("Day of week via Zeller congruence") {
    // 2024-01-01 was a Monday (1)
    CHECK(CalendarHelper::getDayOfWeek(2024, 1, 1) == 1);
    // 2026-11-23 is Monday (1)
    CHECK(CalendarHelper::getDayOfWeek(2026, 11, 23) == 1);
}

TEST_CASE("Reminder manager CRUD and date filtering") {
    ReminderManager manager;
    Reminder r1{"1", 2026, 11, 25, 10, 0, "Sprint Planning", "High", false};
    Reminder r2{"2", 2026, 11, 25, 14, 30, "Doctor Appointment", "Normal", false};
    Reminder r3{"3", 2026, 12, 1, 9, 0, "Monthly Review", "Low", false};

    CHECK(manager.addReminder(r1) == true);
    CHECK(manager.addReminder(r2) == true);
    CHECK(manager.addReminder(r3) == true);

    auto nov25 = manager.getRemindersForDate(2026, 11, 25);
    CHECK(nov25.size() == 2);

    CHECK(manager.markCompleted("1") == true);
    CHECK(manager.getAllReminders()[0].completed == true);

    CHECK(manager.removeReminder("2") == true);
    CHECK(manager.size() == 2);
}

TEST_CASE("File serialization roundtrip") {
    const std::string tempFile = "temp_reminders_test.txt";

    ReminderManager out;
    out.addReminder({"rem_1", 2026, 11, 30, 18, 0, "Conference Keynote", "High", false});
    CHECK(out.saveToFile(tempFile) == true);

    ReminderManager in;
    CHECK(in.loadFromFile(tempFile) == true);
    CHECK(in.size() == 1);
    CHECK(in.getAllReminders()[0].title == "Conference Keynote");

    std::remove(tempFile.c_str());
}
