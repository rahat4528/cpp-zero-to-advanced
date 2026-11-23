#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "NotificationManager.hpp"
#include <chrono>

TEST_CASE("NotificationDispatcher - Post, Dismiss, and Count Lifecycle") {
    notify::NotificationDispatcher dispatcher;
    // Suppress console output during tests
    dispatcher.setAlertCallback([](const notify::NotificationItem&) {});

    CHECK(dispatcher.getActiveCount() == 0);

    int id1 = dispatcher.postNotification("Title 1", "Body 1", notify::NotificationPriority::Normal);
    int id2 = dispatcher.postNotification("Title 2", "Body 2", notify::NotificationPriority::High);
    int id3 = dispatcher.postNotification("Title 3", "Body 3", notify::NotificationPriority::Critical);
    CHECK(id3 > id2);

    CHECK(dispatcher.getActiveCount() == 3);

    auto list = dispatcher.getActiveNotifications();
    CHECK(list.size() == 3);
    CHECK(list[0].id == id1);
    CHECK(list[1].priority == notify::NotificationPriority::High);

    CHECK(dispatcher.dismiss(id2) == true);
    CHECK(dispatcher.getActiveCount() == 2);
    CHECK(dispatcher.dismiss(id2) == false); // Already dismissed

    dispatcher.clearAll();
    CHECK(dispatcher.getActiveCount() == 0);
}

TEST_CASE("NotificationDispatcher - Priority String Conversion") {
    using notify::NotificationPriority;
    CHECK(notify::NotificationDispatcher::priorityToString(NotificationPriority::Low) == "LOW");
    CHECK(notify::NotificationDispatcher::priorityToString(NotificationPriority::Normal) == "NORMAL");
    CHECK(notify::NotificationDispatcher::priorityToString(NotificationPriority::High) == "HIGH");
    CHECK(notify::NotificationDispatcher::priorityToString(NotificationPriority::Critical) == "CRITICAL");
}

TEST_CASE("NotificationDispatcher - Custom Callback Execution") {
    notify::NotificationDispatcher dispatcher;
    std::string capturedTitle;

    dispatcher.setAlertCallback([&](const notify::NotificationItem& item) {
        capturedTitle = item.title;
    });

    dispatcher.postNotification("Build Finished", "All 60 tests passed cleanly.");
    CHECK(capturedTitle == "Build Finished");
}
