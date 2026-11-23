#include "HotelManager.hpp"

namespace hotel {

HotelManager::HotelManager(const std::string& dbPath)
    : m_db(dbPath) {}

bool HotelManager::initialize() {
    return m_db.open();
}

int HotelManager::createRoom(const std::string& roomNum, RoomType type, int64_t priceCents, int occupancy) {
    return m_db.addRoom(roomNum, type, priceCents, occupancy);
}

int HotelManager::createGuest(const std::string& name, const std::string& email, const std::string& phone) {
    return m_db.registerGuest(name, email, phone);
}

std::vector<Room> HotelManager::findAvailable(const std::string& checkIn, const std::string& checkOut, std::optional<RoomType> type) const {
    return m_db.getAvailableRooms(checkIn, checkOut, type);
}

bool HotelManager::bookRoom(int roomId, int guestId, const std::string& checkIn, const std::string& checkOut, std::string& outRef, std::string& outErr) {
    return m_db.createBooking(roomId, guestId, checkIn, checkOut, outRef, outErr);
}

bool HotelManager::cancel(const std::string& bookingRef) {
    return m_db.cancelBooking(bookingRef);
}

} // namespace hotel
