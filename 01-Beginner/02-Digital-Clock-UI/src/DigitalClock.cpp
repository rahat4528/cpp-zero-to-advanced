#include "DigitalClock.h"

#include <array>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace digital_clock {

void convertTo12Hour(int& hours, bool& isPM) {
    isPM = (hours >= 12);
    hours = hours % 12;
    if (hours == 0) {
        hours = 12;
    }
}

FormattedTime formatTime(std::chrono::system_clock::time_point tp,
                         TimeFormat format,
                         int timezoneOffsetHours) {
    // Add timezone offset
    auto adjustedTp = tp + std::chrono::hours(timezoneOffsetHours);
    std::time_t tt = std::chrono::system_clock::to_time_t(adjustedTp);

    std::tm timeInfo{};
#if defined(_WIN32)
    gmtime_s(&timeInfo, &tt);
#else
    gmtime_r(&tt, &timeInfo);
#endif

    FormattedTime ft;
    ft.hours = timeInfo.tm_hour;
    ft.minutes = timeInfo.tm_min;
    ft.seconds = timeInfo.tm_sec;

    std::ostringstream oss;
    if (format == TimeFormat::Format12Hour) {
        convertTo12Hour(ft.hours, ft.isPM);
        oss << std::setfill('0') << std::setw(2) << ft.hours << ":"
            << std::setfill('0') << std::setw(2) << ft.minutes << ":"
            << std::setfill('0') << std::setw(2) << ft.seconds << " "
            << (ft.isPM ? "PM" : "AM");
    } else {
        oss << std::setfill('0') << std::setw(2) << ft.hours << ":"
            << std::setfill('0') << std::setw(2) << ft.minutes << ":"
            << std::setfill('0') << std::setw(2) << ft.seconds;
    }
    ft.timeString = oss.str();
    return ft;
}

// 7-segment digit representations (5 lines high)
static const std::array<std::array<std::string, 5>, 10> kDigits = {{
    /* 0 */ {" _ ", "| |", "| |", "| |", " _ "},
    /* 1 */ {"   ", "  |", "  |", "  |", "   "},
    /* 2 */ {" _ ", "  |", " _ ", "|  ", " _ "},
    /* 3 */ {" _ ", "  |", " _ ", "  |", " _ "},
    /* 4 */ {"   ", "| |", " _ ", "  |", "   "},
    /* 5 */ {" _ ", "|  ", " _ ", "  |", " _ "},
    /* 6 */ {" _ ", "|  ", " _ ", "| |", " _ "},
    /* 7 */ {" _ ", "  |", "   ", "  |", "   "},
    /* 8 */ {" _ ", "| |", " _ ", "| |", " _ "},
    /* 9 */ {" _ ", "| |", " _ ", "  |", " _ "}
}};

static const std::array<std::string, 5> kColon = {
    "   ", " o ", "   ", " o ", "   "
};

static const std::array<std::string, 5> kSpace = {
    "   ", "   ", "   ", "   ", "   "
};

std::string renderSevenSegment(const std::string& timeStr) {
    std::array<std::string, 5> lines;

    for (char ch : timeStr) {
        if (ch >= '0' && ch <= '9') {
            size_t idx = static_cast<size_t>(ch - '0');
            for (size_t row = 0; row < 5; ++row) {
                lines[row] += kDigits[idx][row] + " ";
            }
        } else if (ch == ':') {
            for (size_t row = 0; row < 5; ++row) {
                lines[row] += kColon[row] + " ";
            }
        } else {
            // Space or text characters
            for (size_t row = 0; row < 5; ++row) {
                lines[row] += kSpace[row] + " ";
            }
        }
    }

    std::ostringstream result;
    for (size_t row = 0; row < 5; ++row) {
        result << lines[row] << "\n";
    }
    return result.str();
}

} // namespace digital_clock
