#include "CalendarReminder.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

void printHelp() {
    std::cout << "Calendar Reminder App\n"
              << "Usage:\n"
              << "  CalendarReminder --month <YYYY-MM>               Display monthly calendar grid\n"
              << "  CalendarReminder --add \"YYYY-MM-DD|HH:MM|Title|Priority\" Add reminder\n"
              << "  CalendarReminder --list                          List all reminders\n"
              << "  CalendarReminder --complete <id>                 Mark reminder completed\n"
              << "  CalendarReminder --file <path>                   Specify storage file\n"
              << "  CalendarReminder --help                          Show help\n"
              << "Examples:\n"
              << "  CalendarReminder --month 2026-11\n"
              << "  CalendarReminder --add \"2026-11-25|14:30|Team Standup|High\"\n";
}

void printRemindersTable(const std::vector<calendar_reminder::Reminder>& reminders) {
    if (reminders.empty()) {
        std::cout << "No reminders found.\n";
        return;
    }
    std::cout << "\n========================================================================================\n";
    std::cout << std::left << std::setw(10) << "ID"
              << std::setw(14) << "DATE"
              << std::setw(8)  << "TIME"
              << std::setw(30) << "TITLE"
              << std::setw(12) << "PRIORITY"
              << "STATUS\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    for (const auto& r : reminders) {
        std::ostringstream dateOss, timeOss;
        dateOss << r.year << "-" << std::setfill('0') << std::setw(2) << r.month << "-" << std::setw(2) << r.day;
        timeOss << std::setfill('0') << std::setw(2) << r.hour << ":" << std::setw(2) << r.minute;

        std::cout << std::left << std::setw(10) << r.id
                  << std::setw(14) << dateOss.str()
                  << std::setw(8)  << timeOss.str()
                  << std::setw(30) << r.title
                  << std::setw(12) << r.priority
                  << (r.completed ? "[DONE]" : "[PENDING]") << "\n";
    }
    std::cout << "========================================================================================\n";
}

int main(int argc, char* argv[]) {
    calendar_reminder::ReminderManager manager;
    std::string filePath = "reminders.txt";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filePath = argv[++i];
        }
    }

    manager.loadFromFile(filePath);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--month" && i + 1 < argc) {
            std::string monthStr = argv[++i];
            int y = 2026, m = 11;
            char dash;
            std::istringstream(monthStr) >> y >> dash >> m;
            std::cout << calendar_reminder::CalendarHelper::renderMonthGrid(y, m);
            return 0;
        } else if (arg == "--list") {
            printRemindersTable(manager.getAllReminders());
            return 0;
        } else if (arg == "--complete" && i + 1 < argc) {
            std::string id = argv[++i];
            if (manager.markCompleted(id)) {
                manager.saveToFile(filePath);
                std::cout << "Reminder " << id << " marked as completed.\n";
            } else {
                std::cerr << "Reminder " << id << " not found.\n";
                return 1;
            }
            return 0;
        } else if (arg == "--add" && i + 1 < argc) {
            std::string payload = argv[++i];
            std::stringstream ss(payload);
            std::string datePart, timePart, titlePart, priorityPart;
            std::getline(ss, datePart, '|');
            std::getline(ss, timePart, '|');
            std::getline(ss, titlePart, '|');
            std::getline(ss, priorityPart);

            calendar_reminder::Reminder r;
            char dash, colon;
            std::istringstream(datePart) >> r.year >> dash >> r.month >> dash >> r.day;
            std::istringstream(timePart) >> r.hour >> colon >> r.minute;
            r.title = titlePart;
            r.priority = priorityPart.empty() ? "Normal" : priorityPart;

            if (manager.addReminder(r)) {
                manager.saveToFile(filePath);
                std::cout << "Reminder '" << r.title << "' added successfully.\n";
            } else {
                std::cerr << "Error: Invalid reminder date or title.\n";
                return 1;
            }
            return 0;
        }
    }

    // Default: display current month grid and reminders
    std::cout << calendar_reminder::CalendarHelper::renderMonthGrid(2026, 11);
    printRemindersTable(manager.getAllReminders());
    return 0;
}
