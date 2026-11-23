#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <sqlite3.h>

namespace hotel {

enum class RoomType {
    Single = 0,
    Double = 1,
    Suite = 2,
    Penthouse = 3
};

struct Room {
    int id{0};
    std::string roomNumber;
    RoomType type{RoomType::Single};
    int64_t basePriceCents{0};
    int maxOccupancy{2};
    std::string status{"Active"};
};

struct Guest {
    int id{0};
    std::string name;
    std::string email;
    std::string phone;
};

struct Booking {
    int id{0};
    std::string bookingRef;
    int roomId{0};
    std::string roomNumber;
    int guestId{0};
    std::string guestName;
    std::string checkInDate;  // YYYY-MM-DD
    std::string checkOutDate; // YYYY-MM-DD
    int64_t totalCents{0};
    std::string status{"CONFIRMED"};
};

class HotelDatabase {
public:
    explicit HotelDatabase(const std::string& dbPath = ":memory:");
    ~HotelDatabase();

    bool open();
    void close();
    [[nodiscard]] bool isOpen() const noexcept;

    bool initializeSchema();

    int addRoom(const std::string& roomNumber, RoomType type, int64_t basePriceCents, int maxOccupancy = 2);
    int registerGuest(const std::string& name, const std::string& email, const std::string& phone);

    [[nodiscard]] std::vector<Room> getAllRooms() const;
    [[nodiscard]] std::vector<Room> getAvailableRooms(const std::string& checkIn, const std::string& checkOut, std::optional<RoomType> type = std::nullopt) const;

    bool createBooking(int roomId, int guestId, const std::string& checkIn, const std::string& checkOut, std::string& outRef, std::string& outErr);
    bool cancelBooking(const std::string& bookingRef);
    [[nodiscard]] std::vector<Booking> getBookingsForRoom(int roomId) const;
    [[nodiscard]] std::optional<Booking> getBookingByRef(const std::string& bookingRef) const;

    [[nodiscard]] static std::string roomTypeToString(RoomType type);

private:
    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace hotel
