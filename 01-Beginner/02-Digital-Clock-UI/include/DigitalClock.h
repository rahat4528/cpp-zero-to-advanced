#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace digital_clock {

enum class TimeFormat {
    Format24Hour,
    Format12Hour
};

struct FormattedTime {
    int hours{0};
    int minutes{0};
    int seconds{0};
    bool isPM{false};
    std::string timeString; // e.g. "14:05:09" or "02:05:09 PM"
};

// Formats a given system time_point into hours, minutes, seconds and string representation
FormattedTime formatTime(std::chrono::system_clock::time_point tp,
                         TimeFormat format = TimeFormat::Format24Hour,
                         int timezoneOffsetHours = 0);

// Helper function converting hours into 12-hour format and setting isPM flag
void convertTo12Hour(int& hours, bool& isPM);

// Renders a time string (e.g. "12:45:00") into a multi-line ASCII 7-segment digital display banner
std::string renderSevenSegment(const std::string& timeStr);

} // namespace digital_clock
