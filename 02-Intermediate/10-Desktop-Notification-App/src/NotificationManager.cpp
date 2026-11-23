#include "NotificationManager.hpp"
#include <iostream>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

namespace notify {

NotificationDispatcher::NotificationDispatcher() {
    // Default callback: console alert banner
    m_callback = [](const NotificationItem& item) {
        std::cout << "\n[DESKTOP NOTIFICATION - " << priorityToString(item.priority) << "]\n";
        std::cout << "  Title  : " << item.title << "\n";
        std::cout << "  Message: " << item.message << "\n\n";
    };
}

NotificationDispatcher::~NotificationDispatcher() {
    m_active = false;
    for (auto& t : m_scheduledThreads) {
        if (t.joinable()) {
            t.detach();
        }
    }
}

std::string NotificationDispatcher::priorityToString(NotificationPriority p) {
    switch (p) {
        case NotificationPriority::Low: return "LOW";
        case NotificationPriority::Normal: return "NORMAL";
        case NotificationPriority::High: return "HIGH";
        case NotificationPriority::Critical: return "CRITICAL";
    }
    return "NORMAL";
}

int NotificationDispatcher::postNotification(const std::string& title, const std::string& message,
                                            NotificationPriority priority, int durationSeconds) {
    NotificationItem item;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        item.id = m_nextId++;
        item.title = title;
        item.message = message;
        item.priority = priority;
        item.timestamp = std::chrono::system_clock::now();
        item.durationSeconds = durationSeconds;
        item.isDismissed = false;
        m_notifications.push_back(item);
    }

    triggerNativeAlert(item);

    if (m_callback) {
        m_callback(item);
    }

    return item.id;
}

int NotificationDispatcher::scheduleNotification(const std::string& title, const std::string& message,
                                                int delaySeconds, NotificationPriority priority) {
    int id = 0;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        id = m_nextId++;
    }

    m_scheduledThreads.emplace_back([this, title, message, delaySeconds, priority, id]() {
        std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
        if (!m_active) return;

        NotificationItem item;
        item.id = id;
        item.title = title;
        item.message = message;
        item.priority = priority;
        item.timestamp = std::chrono::system_clock::now();
        item.durationSeconds = 5;
        item.isDismissed = false;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_notifications.push_back(item);
        }

        triggerNativeAlert(item);

        if (m_callback) {
            m_callback(item);
        }
    });

    return id;
}

bool NotificationDispatcher::dismiss(int id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& n : m_notifications) {
        if (n.id == id && !n.isDismissed) {
            n.isDismissed = true;
            return true;
        }
    }
    return false;
}

void NotificationDispatcher::clearAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_notifications.clear();
}

std::vector<NotificationItem> NotificationDispatcher::getActiveNotifications() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<NotificationItem> active;
    for (const auto& n : m_notifications) {
        if (!n.isDismissed) {
            active.push_back(n);
        }
    }
    return active;
}

size_t NotificationDispatcher::getActiveCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::count_if(m_notifications.begin(), m_notifications.end(),
                         [](const NotificationItem& n) { return !n.isDismissed; });
}

void NotificationDispatcher::setAlertCallback(AlertCallback cb) {
    m_callback = std::move(cb);
}

void NotificationDispatcher::triggerNativeAlert(const NotificationItem& item) {
#if defined(_WIN32) || defined(_WIN64)
    // On Windows, if critical or high, play sound alert
    if (item.priority == NotificationPriority::Critical) {
        MessageBeep(MB_ICONERROR);
    } else if (item.priority == NotificationPriority::High) {
        MessageBeep(MB_ICONWARNING);
    }
#else
    (void)item;
#endif
}

} // namespace notify
