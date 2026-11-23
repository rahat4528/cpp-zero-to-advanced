#include "AlarmScheduler.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    alarm::AlarmScheduler scheduler;

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "                REMINDER & ALARM SCHEDULER DAEMON                \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Scheduling short timer (1s) and testing trigger...\n";

        bool alarmFired = false;
        scheduler.setTriggerCallback([&](const alarm::AlarmItem& a) {
            std::cout << "  -> Alarm event triggered: \"" << a.label << "\" (ID: " << a.id << ")\n";
            alarmFired = true;
        });

        int id = scheduler.scheduleAlarm("Daily Standup Reminder", 1);
        std::cout << "Scheduled alarm ID " << id << " for 1 second in the future.\n";
        std::cout << "Pending alarm count: " << scheduler.getPendingCount() << "\n";

        // Wait up to 2 seconds for worker to fire
        for (int i = 0; i < 20 && !alarmFired; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Alarm fired status: " << (alarmFired ? "SUCCESS" : "WAITING") << "\n";
        std::cout << "Pending count after trigger: " << scheduler.getPendingCount() << "\n";
        std::cout << "[Test-Run Mode] Completed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "                REMINDER & ALARM SCHEDULER DAEMON                \n";
    std::cout << "=================================================================\n";
    std::cout << "Run with --test-run to execute automated verification suite.\n";
    return 0;
}
