#include "AlarmScheduler.hpp"
#include <iostream>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

namespace alarm {

AlarmScheduler::AlarmScheduler() {
    m_callback = [](const AlarmItem& a) {
        std::cout << "\n>>> [ALARM RINGING] <<<\n";
        std::cout << "  ID   : " << a.id << "\n";
        std::cout << "  Label: " << a.label << "\n\n";
    };
    startWorker();
}

AlarmScheduler::~AlarmScheduler() {
    stopWorker();
}

void AlarmScheduler::startWorker() {
    if (m_running) return;
    m_running = true;
    m_workerThread = std::thread(&AlarmScheduler::workerLoop, this);
}

void AlarmScheduler::stopWorker() {
    if (!m_running) return;
    m_running = false;
    m_cv.notify_all();
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

int AlarmScheduler::scheduleAlarm(const std::string& label, int delaySeconds,
                                 bool isRecurring, int intervalSeconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    int id = m_nextId++;
    AlarmItem a;
    a.id = id;
    a.label = label;
    a.triggerTime = std::chrono::system_clock::now() + std::chrono::seconds(delaySeconds);
    a.isRecurring = isRecurring;
    a.recurringIntervalSeconds = intervalSeconds;
    a.isTriggered = false;
    a.isDismissed = false;

    m_alarms.push_back(a);
    m_cv.notify_one();
    return id;
}

bool AlarmScheduler::snooze(int id, int snoozeSeconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& a : m_alarms) {
        if (a.id == id && !a.isDismissed) {
            a.triggerTime = std::chrono::system_clock::now() + std::chrono::seconds(snoozeSeconds);
            a.isTriggered = false;
            m_cv.notify_one();
            return true;
        }
    }
    return false;
}

bool AlarmScheduler::dismiss(int id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& a : m_alarms) {
        if (a.id == id && !a.isDismissed) {
            a.isDismissed = true;
            return true;
        }
    }
    return false;
}

void AlarmScheduler::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_alarms.clear();
    m_cv.notify_one();
}

size_t AlarmScheduler::getPendingCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::count_if(m_alarms.begin(), m_alarms.end(), [](const AlarmItem& a) {
        return !a.isTriggered && !a.isDismissed;
    });
}

std::vector<AlarmItem> AlarmScheduler::getAlarms() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_alarms;
}

void AlarmScheduler::setTriggerCallback(AlarmCallback cb) {
    m_callback = std::move(cb);
}

void AlarmScheduler::playAcousticAlert() {
#if defined(_WIN32) || defined(_WIN64)
    MessageBeep(MB_ICONEXCLAMATION);
#endif
}

void AlarmScheduler::workerLoop() {
    while (m_running) {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_alarms.empty()) {
            m_cv.wait(lock, [this]() { return !m_running || !m_alarms.empty(); });
            if (!m_running) break;
        }

        auto now = std::chrono::system_clock::now();
        auto nextTrigger = now + std::chrono::hours(24);
        bool hasPending = false;

        for (const auto& a : m_alarms) {
            if (!a.isTriggered && !a.isDismissed) {
                hasPending = true;
                if (a.triggerTime < nextTrigger) {
                    nextTrigger = a.triggerTime;
                }
            }
        }

        if (!hasPending) {
            m_cv.wait(lock, [this]() {
                if (!m_running) return true;
                return std::any_of(m_alarms.begin(), m_alarms.end(), [](const AlarmItem& a) {
                    return !a.isTriggered && !a.isDismissed;
                });
            });
            if (!m_running) break;
            continue;
        }

        if (nextTrigger > now) {
            m_cv.wait_until(lock, nextTrigger, [this, nextTrigger]() {
                return !m_running || (std::chrono::system_clock::now() >= nextTrigger);
            });
            if (!m_running) break;
        }

        now = std::chrono::system_clock::now();
        std::vector<AlarmItem> triggeredList;

        for (auto& a : m_alarms) {
            if (!a.isTriggered && !a.isDismissed && a.triggerTime <= now) {
                a.isTriggered = true;
                triggeredList.push_back(a);

                if (a.isRecurring && a.recurringIntervalSeconds > 0) {
                    a.triggerTime = now + std::chrono::seconds(a.recurringIntervalSeconds);
                    a.isTriggered = false;
                }
            }
        }

        lock.unlock();

        for (const auto& a : triggeredList) {
            playAcousticAlert();
            if (m_callback) {
                m_callback(a);
            }
        }
    }
}

} // namespace alarm
