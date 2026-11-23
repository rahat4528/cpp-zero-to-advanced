#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "AlarmScheduler.hpp"
#include <atomic>

TEST_CASE("AlarmScheduler - Schedule, Snooze, and Dismiss Lifecycle") {
    alarm::AlarmScheduler scheduler;
    scheduler.setTriggerCallback([](const alarm::AlarmItem&) {}); // Silent

    CHECK(scheduler.getPendingCount() == 0);

    // Schedule 2 alarms far in future
    int id1 = scheduler.scheduleAlarm("Alarm A", 3600);
    int id2 = scheduler.scheduleAlarm("Alarm B", 7200);

    CHECK(scheduler.getPendingCount() == 2);

    CHECK(scheduler.dismiss(id1) == true);
    CHECK(scheduler.getPendingCount() == 1);
    CHECK(scheduler.dismiss(id1) == false); // Already dismissed

    CHECK(scheduler.snooze(id2, 1800) == true);
    CHECK(scheduler.getPendingCount() == 1);

    scheduler.clear();
    CHECK(scheduler.getPendingCount() == 0);
}

TEST_CASE("AlarmScheduler - Trigger Execution") {
    alarm::AlarmScheduler scheduler;
    std::atomic<bool> triggered{false};
    std::string firedLabel;

    scheduler.setTriggerCallback([&](const alarm::AlarmItem& a) {
        firedLabel = a.label;
        triggered = true;
    });

    // Schedule for 1 second
    scheduler.scheduleAlarm("Quick Check", 1);

    // Wait up to 2 seconds
    for (int i = 0; i < 20 && !triggered; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    CHECK(triggered == true);
    CHECK(firedLabel == "Quick Check");
}
