#include "RentalSystem.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace rental {

RentalDatabase::RentalDatabase(const std::string& dbPath) : m_dbPath(dbPath) {}

RentalDatabase::~RentalDatabase() {
    close();
}

bool RentalDatabase::open() {
    if (m_db) return true;
    if (sqlite3_open(m_dbPath.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "SQLite open error: " << sqlite3_errmsg(m_db) << "\n";
        m_db = nullptr;
        return false;
    }
    initSchema();
    return true;
}

void RentalDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

void RentalDatabase::initSchema() {
    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS vehicles (
            vin TEXT PRIMARY KEY,
            make TEXT NOT NULL,
            model TEXT NOT NULL,
            year INTEGER NOT NULL,
            category TEXT NOT NULL,
            daily_rate REAL NOT NULL
        );
        CREATE TABLE IF NOT EXISTS customers (
            license_number TEXT PRIMARY KEY,
            full_name TEXT NOT NULL,
            phone TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS reservations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            code TEXT UNIQUE NOT NULL,
            vin TEXT NOT NULL,
            license_number TEXT NOT NULL,
            start_date TEXT NOT NULL,
            end_date TEXT NOT NULL,
            days INTEGER NOT NULL,
            total_cost REAL NOT NULL,
            status TEXT NOT NULL,
            FOREIGN KEY(vin) REFERENCES vehicles(vin),
            FOREIGN KEY(license_number) REFERENCES customers(license_number)
        );
        CREATE INDEX IF NOT EXISTS idx_res_dates ON reservations(vin, start_date, end_date, status);
    )";

    char* err = nullptr;
    if (sqlite3_exec(m_db, schema, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string errStr = err ? err : "Unknown error";
        sqlite3_free(err);
        throw std::runtime_error("Failed to initialize rental schema: " + errStr);
    }
}

bool RentalDatabase::registerVehicle(const Vehicle& vehicle) {
    if (!m_db) return false;

    const char* sql = "INSERT OR REPLACE INTO vehicles (vin, make, model, year, category, daily_rate) "
                      "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, vehicle.getVin().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, vehicle.getMake().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, vehicle.getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, vehicle.getYear());
    sqlite3_bind_text(stmt, 5, vehicle.getCategoryName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 6, vehicle.getBaseDailyRate());

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool RentalDatabase::registerCustomer(const Customer& customer) {
    if (!m_db) return false;

    const char* sql = "INSERT OR REPLACE INTO customers (license_number, full_name, phone) "
                      "VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, customer.licenseNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, customer.fullName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, customer.phone.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool RentalDatabase::isVehicleAvailable(const std::string& vin, const std::string& startDate, const std::string& endDate) const {
    if (!m_db) return false;

    // A collision occurs if (start <= existing_end) AND (end >= existing_start) for status == 'ACTIVE'
    const char* sql = "SELECT COUNT(*) FROM reservations "
                      "WHERE vin = ? AND status = 'ACTIVE' "
                      "AND (? <= end_date AND ? >= start_date);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, startDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, endDate.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    return count == 0;
}

std::optional<std::string> RentalDatabase::bookReservation(const std::string& vin, const std::string& licenseNumber,
                                                           const std::string& startDate, const std::string& endDate,
                                                           int days, int estimatedMileage, bool insurance) {
    if (!m_db) return std::nullopt;

    if (!isVehicleAvailable(vin, startDate, endDate)) {
        return std::nullopt; // Already booked during this timeframe
    }

    // Query vehicle category and rate
    const char* vSql = "SELECT category, make, model, year, daily_rate FROM vehicles WHERE vin = ?;";
    sqlite3_stmt* vStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, vSql, -1, &vStmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_text(vStmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);

    std::string cat, make, model;
    int year = 0;
    double rate = 0.0;
    if (sqlite3_step(vStmt) == SQLITE_ROW) {
        cat = reinterpret_cast<const char*>(sqlite3_column_text(vStmt, 0));
        make = reinterpret_cast<const char*>(sqlite3_column_text(vStmt, 1));
        model = reinterpret_cast<const char*>(sqlite3_column_text(vStmt, 2));
        year = sqlite3_column_int(vStmt, 3);
        rate = sqlite3_column_double(vStmt, 4);
    } else {
        sqlite3_finalize(vStmt);
        return std::nullopt;
    }
    sqlite3_finalize(vStmt);

    auto vehicleObj = createVehicle(cat, vin, make, model, year, rate);
    double cost = vehicleObj->calculateRentalCost(days, estimatedMileage, insurance);

    // Generate unique code
    static int codeGen = 1000;
    std::string resCode = "RES-" + vin.substr(0, 3) + "-" + std::to_string(++codeGen);

    const char* insSql = "INSERT INTO reservations (code, vin, license_number, start_date, end_date, days, total_cost, status) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?, 'ACTIVE');";
    sqlite3_stmt* insStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, insSql, -1, &insStmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_text(insStmt, 1, resCode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insStmt, 2, vin.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insStmt, 3, licenseNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insStmt, 4, startDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insStmt, 5, endDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(insStmt, 6, days);
    sqlite3_bind_double(insStmt, 7, cost);

    bool ok = (sqlite3_step(insStmt) == SQLITE_DONE);
    sqlite3_finalize(insStmt);

    if (ok) return resCode;
    return std::nullopt;
}

bool RentalDatabase::returnVehicle(const std::string& reservationCode) {
    if (!m_db) return false;

    const char* sql = "UPDATE reservations SET status = 'RETURNED' WHERE code = ? AND status = 'ACTIVE';";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, reservationCode.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);

    return ok && (changes > 0);
}

std::vector<Reservation> RentalDatabase::getActiveReservations() const {
    std::vector<Reservation> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, code, vin, license_number, start_date, end_date, days, total_cost, status "
                      "FROM reservations WHERE status = 'ACTIVE' ORDER BY start_date ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Reservation r;
        r.id = sqlite3_column_int(stmt, 0);
        r.reservationCode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        r.vin = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        r.licenseNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        r.startDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        r.endDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        r.days = sqlite3_column_int(stmt, 6);
        r.totalCost = sqlite3_column_double(stmt, 7);
        r.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        list.push_back(r);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<std::string> RentalDatabase::getAllVins() const {
    std::vector<std::string> vins;
    if (!m_db) return vins;

    const char* sql = "SELECT vin FROM vehicles ORDER BY vin;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            vins.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
        sqlite3_finalize(stmt);
    }
    return vins;
}

} // namespace rental
