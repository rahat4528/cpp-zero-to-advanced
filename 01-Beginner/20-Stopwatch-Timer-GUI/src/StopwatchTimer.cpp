#include "StopwatchTimer.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace stopwatch_timer {

void StopwatchTimer::start() {
    if (m_state == StopwatchState::Stopped) {
        m_accumulated = std::chrono::nanoseconds{0};
        m_laps.clear();
        m_startTime = std::chrono::steady_clock::now();
        m_state = StopwatchState::Running;
    }
}

void StopwatchTimer::pause() {
    if (m_state == StopwatchState::Running) {
        auto now = std::chrono::steady_clock::now();
        m_accumulated += (now - m_startTime);
        m_pauseTime = now;
        m_state = StopwatchState::Paused;
    }
}

void StopwatchTimer::resume() {
    if (m_state == StopwatchState::Paused) {
        m_startTime = std::chrono::steady_clock::now();
        m_state = StopwatchState::Running;
    }
}

void StopwatchTimer::stop() {
    if (m_state == StopwatchState::Running) {
        auto now = std::chrono::steady_clock::now();
        m_accumulated += (now - m_startTime);
    }
    m_state = StopwatchState::Stopped;
}

void StopwatchTimer::reset() {
    m_state = StopwatchState::Stopped;
    m_accumulated = std::chrono::nanoseconds{0};
    m_laps.clear();
}

double StopwatchTimer::getElapsedSeconds() const {
    if (m_state == StopwatchState::Running) {
        auto now = std::chrono::steady_clock::now();
        auto total = m_accumulated + (now - m_startTime);
        return std::chrono::duration<double>(total).count();
    }
    return std::chrono::duration<double>(m_accumulated).count();
}

int64_t StopwatchTimer::getElapsedMilliseconds() const {
    return static_cast<int64_t>(getElapsedSeconds() * 1000.0);
}

std::optional<LapRecord> StopwatchTimer::recordLap() {
    if (m_state == StopwatchState::Stopped) return std::nullopt;

    double currentSplit = getElapsedSeconds();
    double previousSplit = m_laps.empty() ? 0.0 : m_laps.back().splitTimeSeconds;
    double delta = currentSplit - previousSplit;

    LapRecord rec;
    rec.lapNumber = m_laps.size() + 1;
    rec.splitTimeSeconds = currentSplit;
    rec.lapDeltaSeconds = std::max(0.0, delta);

    m_laps.push_back(rec);
    return rec;
}

std::string StopwatchTimer::formatDuration(double seconds) {
    if (seconds < 0.0) seconds = 0.0;
    int64_t totalMs = static_cast<int64_t>(std::round(seconds * 1000.0));

    int64_t hours = totalMs / 3600000;
    totalMs %= 3600000;

    int64_t minutes = totalMs / 60000;
    totalMs %= 60000;

    int64_t secs = totalMs / 1000;
    int64_t ms = totalMs % 1000;

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << hours << ":"
        << std::setw(2) << minutes << ":"
        << std::setw(2) << secs << "."
        << std::setw(3) << ms;
    return oss.str();
}

std::string StopwatchTimer::getFormattedTime() const {
    return formatDuration(getElapsedSeconds());
}

std::optional<size_t> StopwatchTimer::getFastestLapIndex() const {
    if (m_laps.empty()) return std::nullopt;
    size_t minIdx = 0;
    double minDelta = m_laps[0].lapDeltaSeconds;
    for (size_t i = 1; i < m_laps.size(); ++i) {
        if (m_laps[i].lapDeltaSeconds < minDelta) {
            minDelta = m_laps[i].lapDeltaSeconds;
            minIdx = i;
        }
    }
    return minIdx;
}

std::optional<size_t> StopwatchTimer::getSlowestLapIndex() const {
    if (m_laps.empty()) return std::nullopt;
    size_t maxIdx = 0;
    double maxDelta = m_laps[0].lapDeltaSeconds;
    for (size_t i = 1; i < m_laps.size(); ++i) {
        if (m_laps[i].lapDeltaSeconds > maxDelta) {
            maxDelta = m_laps[i].lapDeltaSeconds;
            maxIdx = i;
        }
    }
    return maxIdx;
}

} // namespace stopwatch_timer
