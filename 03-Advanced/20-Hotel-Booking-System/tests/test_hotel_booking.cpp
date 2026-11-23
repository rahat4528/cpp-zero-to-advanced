#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "HotelManager.hpp"

TEST_CASE("Hotel Manager - Room & Guest Registration") {
    hotel::HotelManager manager(":memory:");
    REQUIRE(manager.initialize());

    int r1 = manager.createRoom("101", hotel::RoomType::Single, 8000, 1);
    int r2 = manager.createRoom("201", hotel::RoomType::Double, 12000, 2);
    int r3 = manager.createRoom("301", hotel::RoomType::Suite, 25000, 4);

    CHECK(r1 > 0);
    CHECK(r2 > 0);
    CHECK(r3 > 0);

    auto allRooms = manager.getDatabase().getAllRooms();
    CHECK(allRooms.size() == 3);

    int g1 = manager.createGuest("Alice Wonderland", "alice@wonder.org", "555-1234");
    int g2 = manager.createGuest("Bob Builder", "bob@builder.org", "555-5678");

    CHECK(g1 > 0);
    CHECK(g2 > 0);
}

TEST_CASE("Hotel Reservation - Date Interval Conflict Detection") {
    hotel::HotelManager manager(":memory:");
    REQUIRE(manager.initialize());

    int r101 = manager.createRoom("101", hotel::RoomType::Double, 10000, 2);
    int g1 = manager.createGuest("Guest One", "g1@test.com", "111");
    int g2 = manager.createGuest("Guest Two", "g2@test.com", "222");

    // 1. Initial valid booking
    std::string ref1, err1;
    bool ok1 = manager.bookRoom(r101, g1, "2026-07-10", "2026-07-15", ref1, err1);
    CHECK(ok1);
    CHECK(!ref1.empty());
    CHECK(err1.empty());

    // 2. Exact match interval -> MUST CONFLICT
    std::string ref2, err2;
    bool ok2 = manager.bookRoom(r101, g2, "2026-07-10", "2026-07-15", ref2, err2);
    CHECK(!ok2);
    CHECK(err2.find("already booked") != std::string::npos);

    // 3. Sub-interval (inside) -> MUST CONFLICT
    std::string ref3, err3;
    bool ok3 = manager.bookRoom(r101, g2, "2026-07-11", "2026-07-14", ref3, err3);
    CHECK(!ok3);

    // 4. Overlap start -> MUST CONFLICT
    std::string ref4, err4;
    bool ok4 = manager.bookRoom(r101, g2, "2026-07-08", "2026-07-12", ref4, err4);
    CHECK(!ok4);

    // 5. Overlap end -> MUST CONFLICT
    std::string ref5, err5;
    bool ok5 = manager.bookRoom(r101, g2, "2026-07-13", "2026-07-18", ref5, err5);
    CHECK(!ok5);

    // 6. Enclosing interval -> MUST CONFLICT
    std::string ref6, err6;
    bool ok6 = manager.bookRoom(r101, g2, "2026-07-05", "2026-07-20", ref6, err6);
    CHECK(!ok6);

    // 7. Non-overlapping: Prior checkout equals new checkin -> VALID
    std::string ref7, err7;
    bool ok7 = manager.bookRoom(r101, g2, "2026-07-15", "2026-07-20", ref7, err7);
    CHECK(ok7);
    CHECK(!ref7.empty());

    // 8. Non-overlapping: New checkout equals prior checkin -> VALID
    std::string ref8, err8;
    bool ok8 = manager.bookRoom(r101, g2, "2026-07-05", "2026-07-10", ref8, err8);
    CHECK(ok8);
    CHECK(!ref8.empty());

    // 9. Invalid date order (checkOut <= checkIn) -> REJECTED
    std::string ref9, err9;
    bool ok9 = manager.bookRoom(r101, g2, "2026-08-10", "2026-08-05", ref9, err9);
    CHECK(!ok9);
    CHECK(err9.find("Check-out date must be strictly after") != std::string::npos);
}

TEST_CASE("Hotel Manager - Cancellation & Availability Search") {
    hotel::HotelManager manager(":memory:");
    REQUIRE(manager.initialize());

    int rSingle = manager.createRoom("101", hotel::RoomType::Single, 7000, 1);
    int rDouble = manager.createRoom("201", hotel::RoomType::Double, 11000, 2);
    int g1 = manager.createGuest("Traveler X", "tx@travel.com", "999");

    // Book single room
    std::string ref, err;
    REQUIRE(manager.bookRoom(rSingle, g1, "2026-09-01", "2026-09-10", ref, err));

    // Availability during booked window:
    // Room 101 should NOT be available, Room 201 should be available
    auto availAll = manager.findAvailable("2026-09-03", "2026-09-07");
    CHECK(availAll.size() == 1);
    CHECK(availAll[0].id == rDouble);

    // Filter by Single type -> should return 0 rooms
    auto availSingle = manager.findAvailable("2026-09-03", "2026-09-07", hotel::RoomType::Single);
    CHECK(availSingle.empty());

    // Filter by Double type -> should return 1 room
    auto availDouble = manager.findAvailable("2026-09-03", "2026-09-07", hotel::RoomType::Double);
    CHECK(availDouble.size() == 1);
    CHECK(availDouble[0].id == rDouble);

    // Cancel reservation for Room 101
    bool cancelled = manager.cancel(ref);
    CHECK(cancelled);

    // Now Room 101 should be available again
    auto availAfterCancel = manager.findAvailable("2026-09-03", "2026-09-07", hotel::RoomType::Single);
    CHECK(availAfterCancel.size() == 1);
    CHECK(availAfterCancel[0].id == rSingle);
}
