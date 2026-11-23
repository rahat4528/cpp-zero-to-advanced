#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace stopwatch_timer {

enum class StopwatchState {
    Stopped,
    Running,
    Paused
};

struct LapRecord {
    size_t lapNumber{0};
    double splitTimeSeconds{0.0}; // Total elapsed time when lap recorded
    double lapDeltaSeconds{0.0};  // Duration of this specific lap
};

class StopwatchTimer {
public:
    StopwatchTimer() = default;

    // Controls
    void start();
    void pause();
    void resume();
    void stop();
    void reset();

    // Lap recording
    std::optional<LapRecord> recordLap();
    const std::vector<LapRecord>& getLaps() const { return m_laps; }

    // Query state and elapsed durations
    StopwatchState getState() const { return m_state; }
    double getElapsedSeconds() const;
    int64_t getElapsedMilliseconds() const;

    // Formatted time string: "HH:MM:SS.mmm"
    std::string getFormattedTime() const;
    static std::string formatDuration(double seconds);

    // Lap analysis
    std::optional<size_t> getFastestLapIndex() const;
    std::optional<size_t> getSlowestLapIndex() const;

private:
    StopwatchState m_state{StopwatchState::Stopped};
    std::chrono::steady_clock::time_point m_startTime;
    std::chrono::steady_clock::time_point m_pauseTime;
    std::chrono::nanoseconds m_accumulated{0};
    std::vector<LapRecord> m_laps;
};

} // namespace stopwatch_timer
