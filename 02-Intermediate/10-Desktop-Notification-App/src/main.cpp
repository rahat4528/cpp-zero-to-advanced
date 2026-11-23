#include "NotificationManager.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    notify::NotificationDispatcher dispatcher;

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "             DESKTOP NOTIFICATION DISPATCHER APP                 \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Dispatching priority notifications and testing lifecycle...\n";

        int id1 = dispatcher.postNotification("Calendar Event", "Team sync in 10 minutes", notify::NotificationPriority::Normal);
        int id2 = dispatcher.postNotification("Low Battery", "15% remaining. Plug in charger.", notify::NotificationPriority::High);
        int id3 = dispatcher.postNotification("Disk Space Critical", "Root partition has < 500 MB remaining!", notify::NotificationPriority::Critical);

        std::cout << "Dispatched 3 notifications: IDs " << id1 << ", " << id2 << ", " << id3 << "\n";
        std::cout << "Active notification count: " << dispatcher.getActiveCount() << "\n";

        dispatcher.dismiss(id1);
        std::cout << "Dismissed ID " << id1 << ". Active remaining: " << dispatcher.getActiveCount() << "\n";

        std::cout << "[Test-Run Mode] Execution completed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "             DESKTOP NOTIFICATION DISPATCHER APP                 \n";
    std::cout << "=================================================================\n";
    std::cout << "Commands:\n";
    std::cout << "  1. Send Info Notification\n";
    std::cout << "  2. Send High Priority Alert\n";
    std::cout << "  3. Send Critical System Warning\n";
    std::cout << "  4. View Active Notifications\n";
    std::cout << "  5. Exit\n";
    std::cout << "-----------------------------------------------------------------\n";

    dispatcher.postNotification("System Started", "Notification dispatcher daemon active.", notify::NotificationPriority::Low);

    return 0;
}
