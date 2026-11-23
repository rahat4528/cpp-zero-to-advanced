#include "CalendarReminder.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace calendar_reminder {

bool CalendarHelper::isLeapYear(int year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    return (year % 4 == 0);
}

int CalendarHelper::getDaysInMonth(int year, int month) {
    if (month < 1 || month > 12) return 0;
    static const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return days[month];
}

// Zeller's Congruence for Gregorian calendar
int CalendarHelper::getDayOfWeek(int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year -= 1;
    }
    int k = year % 100;
    int j = year / 100;
    int h = (day + (13 * (month + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    // Zeller returns: 0 = Saturday, 1 = Sunday, ..., 6 = Friday
    // Convert to 0 = Sunday, 1 = Monday, ..., 6 = Saturday
    return (h + 6) % 7;
}

std::string CalendarHelper::formatDate(int year, int month, int day) {
    std::ostringstream oss;
    oss << year << "-"
        << std::setfill('0') << std::setw(2) << month << "-"
        << std::setfill('0') << std::setw(2) << day;
    return oss.str();
}

std::string CalendarHelper::renderMonthGrid(int year, int month) {
    static const char* monthNames[] = {
        "", "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    int daysInMonth = getDaysInMonth(year, month);
    int startDay = getDayOfWeek(year, month, 1);

    std::ostringstream oss;
    oss << "=====================================\n"
        << "           " << monthNames[month] << " " << year << "\n"
        << "=====================================\n"
        << "  Su   Mo   Tu   We   Th   Fr   Sa   \n"
        << "-------------------------------------\n";

    // Pad leading spaces
    for (int i = 0; i < startDay; ++i) {
        oss << "     ";
    }

    int currentDayOfWeek = startDay;
    for (int day = 1; day <= daysInMonth; ++day) {
        oss << std::setfill(' ') << std::setw(4) << day << " ";
        currentDayOfWeek++;
        if (currentDayOfWeek == 7) {
            oss << "\n";
            currentDayOfWeek = 0;
        }
    }
    if (currentDayOfWeek != 0) {
        oss << "\n";
    }
    oss << "=====================================\n";
    return oss.str();
}

bool ReminderManager::addReminder(const Reminder& reminder) {
    if (reminder.title.empty() || reminder.month < 1 || reminder.month > 12) {
        return false;
    }
    int maxDays = CalendarHelper::getDaysInMonth(reminder.year, reminder.month);
    if (reminder.day < 1 || reminder.day > maxDays) {
        return false;
    }

    Reminder r = reminder;
    if (r.id.empty()) {
        r.id = "rem_" + std::to_string(m_reminders.size() + 1);
    }

    m_reminders.push_back(r);
    // Sort chronologically
    std::sort(m_reminders.begin(), m_reminders.end(), [](const Reminder& a, const Reminder& b) {
        if (a.year != b.year) return a.year < b.year;
        if (a.month != b.month) return a.month < b.month;
        if (a.day != b.day) return a.day < b.day;
        if (a.hour != b.hour) return a.hour < b.hour;
        return a.minute < b.minute;
    });
    return true;
}

bool ReminderManager::removeReminder(const std::string& id) {
    auto it = std::remove_if(m_reminders.begin(), m_reminders.end(), [&](const Reminder& r) {
        return r.id == id;
    });
    if (it != m_reminders.end()) {
        m_reminders.erase(it, m_reminders.end());
        return true;
    }
    return false;
}

bool ReminderManager::markCompleted(const std::string& id) {
    for (auto& r : m_reminders) {
        if (r.id == id) {
            r.completed = true;
            return true;
        }
    }
    return false;
}

std::vector<Reminder> ReminderManager::getAllReminders() const {
    return m_reminders;
}

std::vector<Reminder> ReminderManager::getRemindersForDate(int year, int month, int day) const {
    std::vector<Reminder> matches;
    for (const auto& r : m_reminders) {
        if (r.year == year && r.month == month && r.day == day) {
            matches.push_back(r);
        }
    }
    return matches;
}

std::string ReminderManager::serialize(const Reminder& r) {
    std::ostringstream oss;
    oss << r.id << "|"
        << r.year << "|"
        << r.month << "|"
        << r.day << "|"
        << r.hour << "|"
        << r.minute << "|"
        << r.title << "|"
        << r.priority << "|"
        << (r.completed ? "1" : "0");
    return oss.str();
}

std::optional<Reminder> ReminderManager::deserialize(const std::string& line) {
    std::istringstream iss(line);
    Reminder r;
    std::string yStr, mStr, dStr, hStr, minStr, compStr;

    if (!std::getline(iss, r.id, '|')) return std::nullopt;
    if (!std::getline(iss, yStr, '|')) return std::nullopt;
    if (!std::getline(iss, mStr, '|')) return std::nullopt;
    if (!std::getline(iss, dStr, '|')) return std::nullopt;
    if (!std::getline(iss, hStr, '|')) return std::nullopt;
    if (!std::getline(iss, minStr, '|')) return std::nullopt;
    if (!std::getline(iss, r.title, '|')) return std::nullopt;
    if (!std::getline(iss, r.priority, '|')) return std::nullopt;
    std::getline(iss, compStr);

    try {
        r.year = std::stoi(yStr);
        r.month = std::stoi(mStr);
        r.day = std::stoi(dStr);
        r.hour = std::stoi(hStr);
        r.minute = std::stoi(minStr);
        r.completed = (compStr == "1");
        return r;
    } catch (...) {
        return std::nullopt;
    }
}

bool ReminderManager::saveToFile(const std::string& filepath) const {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;
    out << "id|year|month|day|hour|minute|title|priority|completed\n";
    for (const auto& r : m_reminders) {
        out << serialize(r) << "\n";
    }
    return true;
}

bool ReminderManager::loadFromFile(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) return false;

    std::string line;
    if (std::getline(in, line)) {
        if (line.find("id|year") == std::string::npos) {
            auto rOpt = deserialize(line);
            if (rOpt) addReminder(*rOpt);
        }
    }
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto rOpt = deserialize(line);
        if (rOpt) addReminder(*rOpt);
    }
    return true;
}

} // namespace calendar_reminder
