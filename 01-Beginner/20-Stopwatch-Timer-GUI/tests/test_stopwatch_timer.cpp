#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "StopwatchTimer.h"

#include <thread>

using namespace stopwatch_timer;

TEST_CASE("Initial state is stopped with zero duration") {
    StopwatchTimer sw;
    CHECK(sw.getState() == StopwatchState::Stopped);
    CHECK(sw.getElapsedMilliseconds() == 0);
    CHECK(sw.getFormattedTime() == "00:00:00.000");
}

TEST_CASE("Start and elapsed time accumulation") {
    StopwatchTimer sw;
    sw.start();
    CHECK(sw.getState() == StopwatchState::Running);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    CHECK(sw.getElapsedMilliseconds() >= 40);

    sw.stop();
    CHECK(sw.getState() == StopwatchState::Stopped);
    double stoppedTime = sw.getElapsedSeconds();

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // Time should remain constant after stop
    CHECK(sw.getElapsedSeconds() == doctest::Approx(stoppedTime));
}

TEST_CASE("Pause and resume") {
    StopwatchTimer sw;
    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    sw.pause();
    CHECK(sw.getState() == StopwatchState::Paused);
    double pausedTime = sw.getElapsedSeconds();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    // During pause, elapsed time does not advance
    CHECK(sw.getElapsedSeconds() == doctest::Approx(pausedTime));

    sw.resume();
    CHECK(sw.getState() == StopwatchState::Running);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    CHECK(sw.getElapsedSeconds() > pausedTime);

    sw.stop();
}

TEST_CASE("Lap recording and split time tracking") {
    StopwatchTimer sw;
    sw.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    auto lap1 = sw.recordLap();
    REQUIRE(lap1.has_value());
    CHECK(lap1->lapNumber == 1);
    CHECK(lap1->splitTimeSeconds > 0.0);

    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    auto lap2 = sw.recordLap();
    REQUIRE(lap2.has_value());
    CHECK(lap2->lapNumber == 2);
    CHECK(lap2->splitTimeSeconds > lap1->splitTimeSeconds);
    CHECK(lap2->lapDeltaSeconds > 0.0);

    const auto& laps = sw.getLaps();
    CHECK(laps.size() == 2);

    auto fastest = sw.getFastestLapIndex();
    auto slowest = sw.getSlowestLapIndex();
    CHECK(fastest.has_value());
    CHECK(slowest.has_value());

    sw.reset();
    CHECK(sw.getLaps().empty());
    CHECK(sw.getElapsedMilliseconds() == 0);
}

TEST_CASE("Duration formatting") {
    CHECK(StopwatchTimer::formatDuration(0.0) == "00:00:00.000");
    CHECK(StopwatchTimer::formatDuration(1.234) == "00:00:01.234");
    CHECK(StopwatchTimer::formatDuration(65.5) == "00:01:05.500");
    CHECK(StopwatchTimer::formatDuration(3665.123) == "01:01:05.123");
}
