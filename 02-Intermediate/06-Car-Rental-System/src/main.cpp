#include "RentalSystem.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    std::string dbFile = "car_rental.db";

    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
            dbFile = ":memory:";
        } else if (std::string(argv[i]) == "--db" && i + 1 < argc) {
            dbFile = argv[++i];
        }
    }

    rental::RentalDatabase db(dbFile);
    if (!db.open()) {
        std::cerr << "Failed to initialize car rental database.\n";
        return 1;
    }

    if (testRun) {
        std::cout << "[Test-Run Mode] Populating fleet, booking reservations, and testing collision prevention...\n";

        rental::EconomyCar c1("VIN101", "Honda", "Civic", 2023, 45.0);
        rental::SuvCar c2("VIN202", "Toyota", "RAV4", 2024, 75.0);
        rental::LuxuryCar c3("VIN303", "BMW", "530i", 2024, 150.0);

        db.registerVehicle(c1);
        db.registerVehicle(c2);
        db.registerVehicle(c3);

        rental::Customer cust1{0, "DL-88219", "Sarah Connor", "555-0199"};
        db.registerCustomer(cust1);

        auto code = db.bookReservation("VIN101", "DL-88219", "2026-06-01", "2026-06-05", 4, 300, true);
        if (code) {
            std::cout << "Booking successful! Reservation code: " << *code << "\n";
        }

        // Test collision
        bool avail = db.isVehicleAvailable("VIN101", "2026-06-03", "2026-06-07");
        std::cout << "Is VIN101 available June 3-7? " << (avail ? "YES (Error!)" : "NO (Collision detected correctly)") << "\n";

        auto active = db.getActiveReservations();
        std::cout << "Active reservations: " << active.size() << "\n";

        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "               ENTERPRISE CAR RENTAL FLEET SYSTEM                \n";
    std::cout << "=================================================================\n";

    auto active = db.getActiveReservations();
    std::cout << "Currently Active Bookings: " << active.size() << "\n";
    for (const auto& r : active) {
        std::cout << " [" << r.reservationCode << "] VIN: " << r.vin
                  << " | Customer: " << r.licenseNumber
                  << " | Dates: " << r.startDate << " to " << r.endDate
                  << " | Total: $" << std::fixed << std::setprecision(2) << r.totalCost << "\n";
    }

    return 0;
}
