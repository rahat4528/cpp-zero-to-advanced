#pragma once

#include "HotelDatabase.hpp"
#include <string>
#include <vector>

namespace hotel {

class HotelManager {
public:
    explicit HotelManager(const std::string& dbPath = ":memory:");

    bool initialize();

    int createRoom(const std::string& roomNum, RoomType type, int64_t priceCents, int occupancy = 2);
    int createGuest(const std::string& name, const std::string& email, const std::string& phone);

    [[nodiscard]] std::vector<Room> findAvailable(const std::string& checkIn, const std::string& checkOut, std::optional<RoomType> type = std::nullopt) const;
    bool bookRoom(int roomId, int guestId, const std::string& checkIn, const std::string& checkOut, std::string& outRef, std::string& outErr);
    bool cancel(const std::string& bookingRef);

    HotelDatabase& getDatabase() noexcept { return m_db; }
    [[nodiscard]] const HotelDatabase& getDatabase() const noexcept { return m_db; }

private:
    HotelDatabase m_db;
};

} // namespace hotel
