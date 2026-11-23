#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>

namespace notify {

enum class NotificationPriority {
    Low,
    Normal,
    High,
    Critical
};

struct NotificationItem {
    int id{0};
    std::string title;
    std::string message;
    NotificationPriority priority{NotificationPriority::Normal};
    std::chrono::system_clock::time_point timestamp;
    int durationSeconds{5};
    bool isDismissed{false};
};

class NotificationDispatcher {
public:
    using AlertCallback = std::function<void(const NotificationItem&)>;

    NotificationDispatcher();
    ~NotificationDispatcher();

    int postNotification(const std::string& title, const std::string& message,
                         NotificationPriority priority = NotificationPriority::Normal,
                         int durationSeconds = 5);

    int scheduleNotification(const std::string& title, const std::string& message,
                             int delaySeconds,
                             NotificationPriority priority = NotificationPriority::Normal);

    bool dismiss(int id);
    void clearAll();

    [[nodiscard]] std::vector<NotificationItem> getActiveNotifications() const;
    [[nodiscard]] size_t getActiveCount() const;

    void setAlertCallback(AlertCallback cb);

    // Formatted priority string
    static std::string priorityToString(NotificationPriority p);

private:
    void triggerNativeAlert(const NotificationItem& item);

    mutable std::mutex m_mutex;
    std::vector<NotificationItem> m_notifications;
    int m_nextId{1};
    AlertCallback m_callback;
    std::vector<std::thread> m_scheduledThreads;
    std::atomic<bool> m_active{true};
};

} // namespace notify
