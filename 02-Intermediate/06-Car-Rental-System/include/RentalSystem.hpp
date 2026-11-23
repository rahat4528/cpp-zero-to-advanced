#pragma once

#include "Vehicle.hpp"
#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>

namespace rental {

struct Customer {
    int id{0};
    std::string licenseNumber;
    std::string fullName;
    std::string phone;
};

struct Reservation {
    int id{0};
    std::string reservationCode;
    std::string vin;
    std::string licenseNumber;
    std::string startDate; // YYYY-MM-DD
    std::string endDate;   // YYYY-MM-DD
    int days{1};
    double totalCost{0.0};
    std::string status;    // "ACTIVE", "RETURNED", "CANCELLED"
};

class RentalDatabase {
public:
    explicit RentalDatabase(const std::string& dbPath);
    ~RentalDatabase();

    bool open();
    void close();

    bool registerVehicle(const Vehicle& vehicle);
    bool registerCustomer(const Customer& customer);

    // Checks date range collision for a VIN
    [[nodiscard]] bool isVehicleAvailable(const std::string& vin, const std::string& startDate, const std::string& endDate) const;

    // Books reservation if vehicle is available and customer exists
    std::optional<std::string> bookReservation(const std::string& vin, const std::string& licenseNumber,
                                               const std::string& startDate, const std::string& endDate,
                                               int days, int estimatedMileage, bool insurance);

    bool returnVehicle(const std::string& reservationCode);

    std::vector<Reservation> getActiveReservations() const;
    std::vector<std::string> getAllVins() const;

private:
    void initSchema();

    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace rental
