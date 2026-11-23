#pragma once

#include <cmath>

namespace gps {

class GeoMath {
public:
    static constexpr double EARTH_RADIUS_KM = 6371.0;
    static constexpr double PI = 3.14159265358979323846;

    [[nodiscard]] static constexpr double toRadians(double degrees) noexcept {
        return degrees * (PI / 180.0);
    }

    [[nodiscard]] static constexpr double toDegrees(double radians) noexcept {
        return radians * (180.0 / PI);
    }

    [[nodiscard]] static double haversineDistanceKm(double lat1, double lon1, double lat2, double lon2) {
        double dLat = toRadians(lat2 - lat1);
        double dLon = toRadians(lon2 - lon1);

        double rLat1 = toRadians(lat1);
        double rLat2 = toRadians(lat2);

        double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0) +
                   std::cos(rLat1) * std::cos(rLat2) *
                   std::sin(dLon / 2.0) * std::sin(dLon / 2.0);

        double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
        return EARTH_RADIUS_KM * c;
    }

    [[nodiscard]] static constexpr double knotsToKmh(double knots) noexcept {
        return knots * 1.852;
    }

    [[nodiscard]] static double initialBearingDegrees(double lat1, double lon1, double lat2, double lon2) {
        double rLat1 = toRadians(lat1);
        double rLat2 = toRadians(lat2);
        double dLon = toRadians(lon2 - lon1);

        double y = std::sin(dLon) * std::cos(rLat2);
        double x = std::cos(rLat1) * std::sin(rLat2) -
                   std::sin(rLat1) * std::cos(rLat2) * std::cos(dLon);

        double bearingRad = std::atan2(y, x);
        double bearingDeg = toDegrees(bearingRad);
        return std::fmod((bearingDeg + 360.0), 360.0);
    }
};

} // namespace gps
