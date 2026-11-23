#include "HotelDatabase.hpp"
#include <random>
#include <sstream>
#include <iostream>

namespace hotel {

static std::string generateBookingRef() {
    static const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, sizeof(chars) - 2);
    std::string ref = "HB-";
    for (int i = 0; i < 7; ++i) ref += chars[dist(gen)];
    return ref;
}

std::string HotelDatabase::roomTypeToString(RoomType type) {
    switch (type) {
        case RoomType::Double:    return "Double";
        case RoomType::Suite:     return "Suite";
        case RoomType::Penthouse: return "Penthouse";
        case RoomType::Single:
        default:                  return "Single";
    }
}

HotelDatabase::HotelDatabase(const std::string& dbPath)
    : m_dbPath(dbPath) {}

HotelDatabase::~HotelDatabase() {
    close();
}

bool HotelDatabase::open() {
    if (m_db) return true;
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        close();
        return false;
    }
    return initializeSchema();
}

void HotelDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool HotelDatabase::isOpen() const noexcept {
    return m_db != nullptr;
}

bool HotelDatabase::initializeSchema() {
    if (!m_db) return false;

    const char* schema =
        "CREATE TABLE IF NOT EXISTS rooms ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  room_number TEXT NOT NULL UNIQUE,"
        "  type INTEGER NOT NULL,"
        "  base_price_cents INTEGER NOT NULL,"
        "  max_occupancy INTEGER NOT NULL DEFAULT 2,"
        "  status TEXT NOT NULL DEFAULT 'Active'"
        ");"
        "CREATE TABLE IF NOT EXISTS guests ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  email TEXT NOT NULL UNIQUE,"
        "  phone TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS bookings ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  booking_ref TEXT NOT NULL UNIQUE,"
        "  room_id INTEGER NOT NULL,"
        "  guest_id INTEGER NOT NULL,"
        "  check_in_date TEXT NOT NULL,"
        "  check_out_date TEXT NOT NULL,"
        "  total_cents INTEGER NOT NULL,"
        "  status TEXT NOT NULL DEFAULT 'CONFIRMED',"
        "  FOREIGN KEY (room_id) REFERENCES rooms(id),"
        "  FOREIGN KEY (guest_id) REFERENCES guests(id)"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, schema, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int HotelDatabase::addRoom(const std::string& roomNumber, RoomType type, int64_t basePriceCents, int maxOccupancy) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO rooms (room_number, type, base_price_cents, max_occupancy, status) VALUES (?, ?, ?, ?, 'Active');";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, roomNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, static_cast<int>(type));
    sqlite3_bind_int64(stmt, 3, basePriceCents);
    sqlite3_bind_int(stmt, 4, maxOccupancy);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return static_cast<int>(sqlite3_last_insert_rowid(m_db));
}

int HotelDatabase::registerGuest(const std::string& name, const std::string& email, const std::string& phone) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO guests (name, email, phone) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, phone.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return static_cast<int>(sqlite3_last_insert_rowid(m_db));
}

std::vector<Room> HotelDatabase::getAllRooms() const {
    std::vector<Room> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, room_number, type, base_price_cents, max_occupancy, status FROM rooms ORDER BY id ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Room r;
        r.id = sqlite3_column_int(stmt, 0);
        r.roomNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        r.type = static_cast<RoomType>(sqlite3_column_int(stmt, 2));
        r.basePriceCents = sqlite3_column_int64(stmt, 3);
        r.maxOccupancy = sqlite3_column_int(stmt, 4);
        r.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        list.push_back(r);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<Room> HotelDatabase::getAvailableRooms(const std::string& checkIn, const std::string& checkOut, std::optional<RoomType> type) const {
    std::vector<Room> list;
    if (!m_db) return list;

    // A room is available if it has NO overlapping CONFIRMED bookings:
    // Overlap condition: NOT (b.check_out_date <= ? OR b.check_in_date >= ?)
    std::string sql =
        "SELECT r.id, r.room_number, r.type, r.base_price_cents, r.max_occupancy, r.status "
        "FROM rooms r WHERE r.status = 'Active' "
        "AND r.id NOT IN ("
        "  SELECT b.room_id FROM bookings b "
        "  WHERE b.status = 'CONFIRMED' "
        "  AND NOT (b.check_out_date <= ? OR b.check_in_date >= ?)"
        ") ";

    if (type.has_value()) {
        sql += "AND r.type = ? ";
    }
    sql += "ORDER BY r.base_price_cents ASC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return list;

    sqlite3_bind_text(stmt, 1, checkIn.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, checkOut.c_str(), -1, SQLITE_TRANSIENT);
    if (type.has_value()) {
        sqlite3_bind_int(stmt, 3, static_cast<int>(*type));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Room r;
        r.id = sqlite3_column_int(stmt, 0);
        r.roomNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        r.type = static_cast<RoomType>(sqlite3_column_int(stmt, 2));
        r.basePriceCents = sqlite3_column_int64(stmt, 3);
        r.maxOccupancy = sqlite3_column_int(stmt, 4);
        r.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        list.push_back(r);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool HotelDatabase::createBooking(int roomId, int guestId, const std::string& checkIn, const std::string& checkOut, std::string& outRef, std::string& outErr) {
    if (!m_db) {
        outErr = "Database not open";
        return false;
    }
    if (checkIn >= checkOut) {
        outErr = "Check-out date must be strictly after check-in date";
        return false;
    }

    // ACID transaction
    if (sqlite3_exec(m_db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        outErr = "Could not start transaction";
        return false;
    }

    auto rollback = [this]() {
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
    };

    // 1. Conflict check query within transaction
    const char* conflictSql =
        "SELECT COUNT(*) FROM bookings "
        "WHERE room_id = ? AND status = 'CONFIRMED' "
        "AND NOT (check_out_date <= ? OR check_in_date >= ?);";

    sqlite3_stmt* cStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, conflictSql, -1, &cStmt, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Failed to prepare conflict query";
        return false;
    }

    sqlite3_bind_int(cStmt, 1, roomId);
    sqlite3_bind_text(cStmt, 2, checkIn.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(cStmt, 3, checkOut.c_str(), -1, SQLITE_TRANSIENT);

    int conflictCount = 0;
    if (sqlite3_step(cStmt) == SQLITE_ROW) {
        conflictCount = sqlite3_column_int(cStmt, 0);
    }
    sqlite3_finalize(cStmt);

    if (conflictCount > 0) {
        rollback();
        outErr = "Room is already booked for the selected date interval";
        return false;
    }

    // 2. Fetch room price
    const char* priceSql = "SELECT base_price_cents FROM rooms WHERE id = ?;";
    sqlite3_stmt* pStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, priceSql, -1, &pStmt, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Failed to query room details";
        return false;
    }
    sqlite3_bind_int(pStmt, 1, roomId);

    int64_t basePrice = 0;
    if (sqlite3_step(pStmt) == SQLITE_ROW) {
        basePrice = sqlite3_column_int64(pStmt, 0);
    } else {
        sqlite3_finalize(pStmt);
        rollback();
        outErr = "Room not found";
        return false;
    }
    sqlite3_finalize(pStmt);

    // 3. Insert reservation
    std::string ref = generateBookingRef();
    const char* insSql =
        "INSERT INTO bookings (booking_ref, room_id, guest_id, check_in_date, check_out_date, total_cents, status) "
        "VALUES (?, ?, ?, ?, ?, ?, 'CONFIRMED');";

    sqlite3_stmt* iStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, insSql, -1, &iStmt, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Failed to prepare booking insertion";
        return false;
    }

    sqlite3_bind_text(iStmt, 1, ref.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(iStmt, 2, roomId);
    sqlite3_bind_int(iStmt, 3, guestId);
    sqlite3_bind_text(iStmt, 4, checkIn.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(iStmt, 5, checkOut.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(iStmt, 6, basePrice); // 1 night demo or multiplied

    if (sqlite3_step(iStmt) != SQLITE_DONE) {
        sqlite3_finalize(iStmt);
        rollback();
        outErr = "Failed to insert booking record";
        return false;
    }
    sqlite3_finalize(iStmt);

    if (sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Transaction commit failed";
        return false;
    }

    outRef = ref;
    return true;
}

bool HotelDatabase::cancelBooking(const std::string& bookingRef) {
    if (!m_db) return false;

    const char* sql = "UPDATE bookings SET status = 'CANCELLED' WHERE booking_ref = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, bookingRef.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Booking> HotelDatabase::getBookingsForRoom(int roomId) const {
    std::vector<Booking> list;
    if (!m_db) return list;

    const char* sql =
        "SELECT b.id, b.booking_ref, b.room_id, r.room_number, b.guest_id, g.name, "
        "b.check_in_date, b.check_out_date, b.total_cents, b.status "
        "FROM bookings b JOIN rooms r ON b.room_id = r.id JOIN guests g ON b.guest_id = g.id "
        "WHERE b.room_id = ? ORDER BY b.check_in_date ASC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    sqlite3_bind_int(stmt, 1, roomId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Booking b;
        b.id = sqlite3_column_int(stmt, 0);
        b.bookingRef = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        b.roomId = sqlite3_column_int(stmt, 2);
        b.roomNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        b.guestId = sqlite3_column_int(stmt, 4);
        b.guestName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        b.checkInDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        b.checkOutDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        b.totalCents = sqlite3_column_int64(stmt, 8);
        b.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        list.push_back(b);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::optional<Booking> HotelDatabase::getBookingByRef(const std::string& bookingRef) const {
    if (!m_db) return std::nullopt;

    const char* sql =
        "SELECT b.id, b.booking_ref, b.room_id, r.room_number, b.guest_id, g.name, "
        "b.check_in_date, b.check_out_date, b.total_cents, b.status "
        "FROM bookings b JOIN rooms r ON b.room_id = r.id JOIN guests g ON b.guest_id = g.id "
        "WHERE b.booking_ref = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, bookingRef.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Booking b;
        b.id = sqlite3_column_int(stmt, 0);
        b.bookingRef = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        b.roomId = sqlite3_column_int(stmt, 2);
        b.roomNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        b.guestId = sqlite3_column_int(stmt, 4);
        b.guestName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        b.checkInDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        b.checkOutDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        b.totalCents = sqlite3_column_int64(stmt, 8);
        b.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        sqlite3_finalize(stmt);
        return b;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

} // namespace hotel
