#include "Vehicle.hpp"
#include <algorithm>

namespace rental {

Vehicle::Vehicle(std::string vin, std::string make, std::string model, int year, double baseDailyRate)
    : m_vin(std::move(vin)), m_make(std::move(make)), m_model(std::move(model)), m_year(year), m_baseDailyRate(baseDailyRate) {}

const std::string& Vehicle::getVin() const { return m_vin; }
const std::string& Vehicle::getMake() const { return m_make; }
const std::string& Vehicle::getModel() const { return m_model; }
int Vehicle::getYear() const { return m_year; }
double Vehicle::getBaseDailyRate() const { return m_baseDailyRate; }

// EconomyCar
EconomyCar::EconomyCar(std::string vin, std::string make, std::string model, int year, double baseDailyRate)
    : Vehicle(std::move(vin), std::move(make), std::move(model), year, baseDailyRate) {}

VehicleCategory EconomyCar::getCategory() const { return VehicleCategory::Economy; }
std::string EconomyCar::getCategoryName() const { return "Economy"; }

double EconomyCar::calculateRentalCost(int days, int mileage, bool insurance) const {
    double cost = days * getBaseDailyRate();
    if (insurance) {
        cost += days * 12.0; // standard insurance
    }
    // Free 100 miles/day, $0.15/mile thereafter
    int allowed = days * 100;
    if (mileage > allowed) {
        cost += (mileage - allowed) * 0.15;
    }
    return cost;
}

// SuvCar
SuvCar::SuvCar(std::string vin, std::string make, std::string model, int year, double baseDailyRate)
    : Vehicle(std::move(vin), std::move(make), std::move(model), year, baseDailyRate) {}

VehicleCategory SuvCar::getCategory() const { return VehicleCategory::SUV; }
std::string SuvCar::getCategoryName() const { return "SUV"; }

double SuvCar::calculateRentalCost(int days, int mileage, bool insurance) const {
    double cost = days * getBaseDailyRate();
    if (insurance) {
        cost += days * 18.0; // SUV full coverage
    }
    // Free 150 miles/day, $0.25/mile thereafter
    int allowed = days * 150;
    if (mileage > allowed) {
        cost += (mileage - allowed) * 0.25;
    }
    return cost;
}

// LuxuryCar
LuxuryCar::LuxuryCar(std::string vin, std::string make, std::string model, int year, double baseDailyRate)
    : Vehicle(std::move(vin), std::move(make), std::move(model), year, baseDailyRate) {}

VehicleCategory LuxuryCar::getCategory() const { return VehicleCategory::Luxury; }
std::string LuxuryCar::getCategoryName() const { return "Luxury"; }

double LuxuryCar::calculateRentalCost(int days, int mileage, bool insurance) const {
    double cost = days * getBaseDailyRate();
    if (insurance) {
        cost += days * 35.0; // Premium concierge insurance
    }
    // Free 75 miles/day, $0.50/mile thereafter
    int allowed = days * 75;
    if (mileage > allowed) {
        cost += (mileage - allowed) * 0.50;
    }
    return cost;
}

std::unique_ptr<Vehicle> createVehicle(const std::string& category, const std::string& vin,
                                       const std::string& make, const std::string& model,
                                       int year, double baseDailyRate) {
    if (category == "SUV") {
        return std::make_unique<SuvCar>(vin, make, model, year, baseDailyRate);
    } else if (category == "Luxury") {
        return std::make_unique<LuxuryCar>(vin, make, model, year, baseDailyRate);
    }
    return std::make_unique<EconomyCar>(vin, make, model, year, baseDailyRate);
}

} // namespace rental
