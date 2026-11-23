#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

namespace alarm {

struct AlarmItem {
    int id{0};
    std::string label;
    std::chrono::system_clock::time_point triggerTime;
    bool isRecurring{false};
    int recurringIntervalSeconds{0};
    bool isTriggered{false};
    bool isDismissed{false};
};

class AlarmScheduler {
public:
    using AlarmCallback = std::function<void(const AlarmItem&)>;

    AlarmScheduler();
    ~AlarmScheduler();

    int scheduleAlarm(const std::string& label, int delaySeconds,
                      bool isRecurring = false, int intervalSeconds = 0);

    bool snooze(int id, int snoozeSeconds = 5);
    bool dismiss(int id);
    void clear();

    [[nodiscard]] size_t getPendingCount() const;
    [[nodiscard]] std::vector<AlarmItem> getAlarms() const;

    void setTriggerCallback(AlarmCallback cb);

    void startWorker();
    void stopWorker();

private:
    void workerLoop();
    void playAcousticAlert();

    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::vector<AlarmItem> m_alarms;
    int m_nextId{1};
    std::atomic<bool> m_running{false};
    std::thread m_workerThread;
    AlarmCallback m_callback;
};

} // namespace alarm
