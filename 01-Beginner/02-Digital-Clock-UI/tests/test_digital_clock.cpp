#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "DigitalClock.h"

using namespace digital_clock;

TEST_CASE("12-hour AM/PM conversion") {
    int h0 = 0; bool pm0 = false;
    convertTo12Hour(h0, pm0);
    CHECK(h0 == 12);
    CHECK(pm0 == false); // Midnight 12:00 AM

    int h1 = 1; bool pm1 = false;
    convertTo12Hour(h1, pm1);
    CHECK(h1 == 1);
    CHECK(pm1 == false);

    int h12 = 12; bool pm12 = false;
    convertTo12Hour(h12, pm12);
    CHECK(h12 == 12);
    CHECK(pm12 == true); // Noon 12:00 PM

    int h13 = 13; bool pm13 = false;
    convertTo12Hour(h13, pm13);
    CHECK(h13 == 1);
    CHECK(pm13 == true);

    int h23 = 23; bool pm23 = false;
    convertTo12Hour(h23, pm23);
    CHECK(h23 == 11);
    CHECK(pm23 == true);
}

TEST_CASE("Time formatting with deterministic timestamp") {
    // Epoch timestamp: 2024-01-01 00:00:00 UTC = 1704067200
    auto tp = std::chrono::system_clock::from_time_t(1704067200);

    auto ft24 = formatTime(tp, TimeFormat::Format24Hour, 0);
    CHECK(ft24.hours == 0);
    CHECK(ft24.minutes == 0);
    CHECK(ft24.seconds == 0);
    CHECK(ft24.timeString == "00:00:00");

    auto ft12 = formatTime(tp, TimeFormat::Format12Hour, 0);
    CHECK(ft12.hours == 12);
    CHECK(ft12.isPM == false);
    CHECK(ft12.timeString == "12:00:00 AM");

    // With +5 timezone offset
    auto ftOffset = formatTime(tp, TimeFormat::Format24Hour, 5);
    CHECK(ftOffset.hours == 5);
    CHECK(ftOffset.timeString == "05:00:00");
}

TEST_CASE("Seven-segment digital rendering structure") {
    std::string rendered = renderSevenSegment("12:00:00");
    CHECK(!rendered.empty());

    // Should contain 5 newlines corresponding to the 5 vertical lines of the 7-segment display
    size_t newlines = std::count(rendered.begin(), rendered.end(), '\n');
    CHECK(newlines == 5);
}
