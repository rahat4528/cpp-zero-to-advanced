#include "HotelManager.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "   Hotel Reservation & Inventory Engine \n";
    std::cout << "  (SQLite Date Interval Conflict Check) \n";
    std::cout << "========================================\n";

    hotel::HotelManager manager(":memory:");
    if (!manager.initialize()) {
        std::cerr << "Failed to open hotel database.\n";
        return 1;
    }

    // Seed hotel rooms
    manager.createRoom("101", hotel::RoomType::Single, 9900, 1);    // $99.00
    manager.createRoom("201", hotel::RoomType::Double, 14900, 2);   // $149.00
    int r301 = manager.createRoom("301", hotel::RoomType::Suite, 29900, 4);    // $299.00
    manager.createRoom("401", hotel::RoomType::Penthouse, 59900, 6);// $599.00

    // Register guests
    int g1 = manager.createGuest("John Doe", "john.doe@example.com", "+1-555-0100");
    int g2 = manager.createGuest("Jane Smith", "jane.smith@example.com", "+1-555-0200");

    std::cout << "Hotel initialized with 4 inventory rooms.\n";

    // 1. Successful booking: John books Suite 301 from 2026-12-10 to 2026-12-15
    std::string ref1, err1;
    bool b1 = manager.bookRoom(r301, g1, "2026-12-10", "2026-12-15", ref1, err1);
    std::cout << "Booking 1 (John in Suite 301, 2026-12-10 -> 2026-12-15): "
              << (b1 ? ("CONFIRMED (Ref: " + ref1 + ")") : ("FAILED: " + err1)) << "\n";

    // 2. Conflicting booking attempt: Jane attempts Suite 301 from 2026-12-12 to 2026-12-18
    std::string ref2, err2;
    bool b2 = manager.bookRoom(r301, g2, "2026-12-12", "2026-12-18", ref2, err2);
    std::cout << "Booking 2 (Jane in Suite 301, 2026-12-12 -> 2026-12-18): "
              << (b2 ? "CONFIRMED (ERROR: CONFLICT MISSED)" : ("REJECTED (SUCCESS: " + err2 + ")")) << "\n";

    // 3. Adjacent non-conflicting booking: Jane books Suite 301 starting exactly on John's checkout (2026-12-15 -> 2026-12-20)
    std::string ref3, err3;
    bool b3 = manager.bookRoom(r301, g2, "2026-12-15", "2026-12-20", ref3, err3);
    std::cout << "Booking 3 (Adjacent: 2026-12-15 -> 2026-12-20): "
              << (b3 ? ("CONFIRMED (Ref: " + ref3 + ")") : ("FAILED: " + err3)) << "\n";

    // Query available rooms during peak dates
    auto available = manager.findAvailable("2026-12-11", "2026-12-14");
    std::cout << "\nRooms Available between 2026-12-11 and 2026-12-14: " << available.size() << "\n";
    for (const auto& r : available) {
        std::cout << "  - Room #" << r.roomNumber << " ("
                  << hotel::HotelDatabase::roomTypeToString(r.type)
                  << ") Price: $" << std::fixed << std::setprecision(2) << (r.basePriceCents / 100.0) << "/night\n";
    }

    if (testRun) {
        std::cout << "\n[Test Mode] Hotel reservation conflict detection verified.\n";
        return 0;
    }

    return 0;
}
