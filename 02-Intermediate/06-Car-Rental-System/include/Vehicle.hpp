#pragma once

#include <string>
#include <memory>

namespace rental {

enum class VehicleCategory {
    Economy,
    SUV,
    Luxury
};

class Vehicle {
public:
    Vehicle(std::string vin, std::string make, std::string model, int year, double baseDailyRate);
    virtual ~Vehicle() = default;

    [[nodiscard]] const std::string& getVin() const;
    [[nodiscard]] const std::string& getMake() const;
    [[nodiscard]] const std::string& getModel() const;
    [[nodiscard]] int getYear() const;
    [[nodiscard]] double getBaseDailyRate() const;
    [[nodiscard]] virtual VehicleCategory getCategory() const = 0;
    [[nodiscard]] virtual std::string getCategoryName() const = 0;

    // Polymorphic cost calculation: days, estimated mileage, insurance opted
    [[nodiscard]] virtual double calculateRentalCost(int days, int mileage, bool insurance) const = 0;

private:
    std::string m_vin;
    std::string m_make;
    std::string m_model;
    int m_year;
    double m_baseDailyRate;
};

class EconomyCar : public Vehicle {
public:
    EconomyCar(std::string vin, std::string make, std::string model, int year, double baseDailyRate = 45.0);
    [[nodiscard]] VehicleCategory getCategory() const override;
    [[nodiscard]] std::string getCategoryName() const override;
    [[nodiscard]] double calculateRentalCost(int days, int mileage, bool insurance) const override;
};

class SuvCar : public Vehicle {
public:
    SuvCar(std::string vin, std::string make, std::string model, int year, double baseDailyRate = 75.0);
    [[nodiscard]] VehicleCategory getCategory() const override;
    [[nodiscard]] std::string getCategoryName() const override;
    [[nodiscard]] double calculateRentalCost(int days, int mileage, bool insurance) const override;
};

class LuxuryCar : public Vehicle {
public:
    LuxuryCar(std::string vin, std::string make, std::string model, int year, double baseDailyRate = 140.0);
    [[nodiscard]] VehicleCategory getCategory() const override;
    [[nodiscard]] std::string getCategoryName() const override;
    [[nodiscard]] double calculateRentalCost(int days, int mileage, bool insurance) const override;
};

std::unique_ptr<Vehicle> createVehicle(const std::string& category, const std::string& vin,
                                       const std::string& make, const std::string& model,
                                       int year, double baseDailyRate);

} // namespace rental
