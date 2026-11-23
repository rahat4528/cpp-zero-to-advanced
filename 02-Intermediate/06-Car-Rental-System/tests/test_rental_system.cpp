#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "RentalSystem.hpp"

TEST_CASE("Vehicle Polymorphic Rate Calculation") {
    rental::EconomyCar economy("VIN_E1", "Toyota", "Corolla", 2022, 40.0);
    rental::SuvCar suv("VIN_S1", "Subaru", "Outback", 2023, 70.0);
    rental::LuxuryCar luxury("VIN_L1", "Mercedes", "E350", 2024, 130.0);

    SUBCASE("Economy calculation") {
        // 3 days, 250 miles (allowed: 300), insurance: false
        double cost = economy.calculateRentalCost(3, 250, false);
        CHECK(cost == doctest::Approx(120.0));

        // With insurance: 3 * 12 = 36 extra
        // Over mileage: 350 miles (50 over * 0.15 = 7.50)
        double costWithAddons = economy.calculateRentalCost(3, 350, true);
        CHECK(costWithAddons == doctest::Approx(120.0 + 36.0 + 7.50));
    }

    SUBCASE("SUV calculation") {
        // 2 days, 400 miles (allowed: 300, 100 over * 0.25 = 25.0), insurance: 2 * 18 = 36.0
        double cost = suv.calculateRentalCost(2, 400, true);
        CHECK(cost == doctest::Approx(140.0 + 36.0 + 25.0));
    }

    SUBCASE("Luxury calculation") {
        // 1 day, 100 miles (allowed: 75, 25 over * 0.50 = 12.50), insurance: 35.0
        double cost = luxury.calculateRentalCost(1, 100, true);
        CHECK(cost == doctest::Approx(130.0 + 35.0 + 12.50));
    }
}

TEST_CASE("RentalDatabase - Reservation Collision & Lifecycle") {
    rental::RentalDatabase db(":memory:");
    REQUIRE(db.open() == true);

    rental::EconomyCar c1("VIN_TEST", "Ford", "Focus", 2021, 50.0);
    db.registerVehicle(c1);
    db.registerCustomer({0, "DL_1234", "John Doe", "555-1234"});

    // Initial check: vehicle available
    CHECK(db.isVehicleAvailable("VIN_TEST", "2026-07-10", "2026-07-15") == true);

    // Book reservation July 10 to July 15
    auto code = db.bookReservation("VIN_TEST", "DL_1234", "2026-07-10", "2026-07-15", 5, 200, false);
    REQUIRE(code.has_value());

    // Conflict overlapping inside range
    CHECK(db.isVehicleAvailable("VIN_TEST", "2026-07-12", "2026-07-14") == false);
    // Conflict overlapping beginning
    CHECK(db.isVehicleAvailable("VIN_TEST", "2026-07-08", "2026-07-11") == false);
    // Conflict overlapping end
    CHECK(db.isVehicleAvailable("VIN_TEST", "2026-07-14", "2026-07-18") == false);

    // Non-conflicting dates outside range
    CHECK(db.isVehicleAvailable("VIN_TEST", "2026-07-01", "2026-07-09") == true);
    CHECK(db.isVehicleAvailable("VIN_TEST", "2026-07-16", "2026-07-20") == true);

    // Return vehicle -> clears active collision
    CHECK(db.returnVehicle(*code) == true);
    CHECK(db.isVehicleAvailable("VIN_TEST", "2026-07-10", "2026-07-15") == true);
}
