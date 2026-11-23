#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace calendar_reminder {

struct Reminder {
    std::string id;
    int year{2024};
    int month{1}; // 1 - 12
    int day{1};   // 1 - 31
    int hour{0};  // 0 - 23
    int minute{0};// 0 - 59
    std::string title;
    std::string priority{"Normal"}; // High, Normal, Low
    bool completed{false};
};

class CalendarHelper {
public:
    static bool isLeapYear(int year);
    static int getDaysInMonth(int year, int month);
    // Returns 0 for Sunday, 1 for Monday, ..., 6 for Saturday
    static int getDayOfWeek(int year, int month, int day);
    static std::string renderMonthGrid(int year, int month);
    static std::string formatDate(int year, int month, int day);
};

class ReminderManager {
public:
    ReminderManager() = default;

    bool addReminder(const Reminder& reminder);
    bool removeReminder(const std::string& id);
    bool markCompleted(const std::string& id);

    std::vector<Reminder> getAllReminders() const;
    std::vector<Reminder> getRemindersForDate(int year, int month, int day) const;
    std::vector<Reminder> getUpcomingReminders(int daysAhead) const;

    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);

    static std::string serialize(const Reminder& r);
    static std::optional<Reminder> deserialize(const std::string& line);

    size_t size() const { return m_reminders.size(); }
    void clear() { m_reminders.clear(); }

private:
    std::vector<Reminder> m_reminders;
};

} // namespace calendar_reminder
