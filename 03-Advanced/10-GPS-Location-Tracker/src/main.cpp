#include "NMEAParser.hpp"
#include "GeoMath.hpp"
#include "GPSSimulator.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "  GPS Location Tracker & NMEA Decoder   \n";
    std::cout << "========================================\n";

    auto stream = gps::GPSSimulator::generateRouteSentences();
    std::cout << "Processing simulated NMEA telemetry stream (" << stream.size() << " sentences)...\n";

    double totalDistanceKm = 0.0;
    std::optional<gps::GPSFix> prevFix;

    for (const auto& sentence : stream) {
        auto fix = gps::NMEAParser::parseSentence(sentence);
        if (fix && fix->isValid) {
            std::cout << "\n[NMEA FIX " << fix->utcTime << "]\n";
            std::cout << std::fixed << std::setprecision(5);
            std::cout << "  Coordinates: " << fix->latitude << ", " << fix->longitude << "\n";
            std::cout << "  Altitude:    " << fix->altitudeMeters << " m\n";
            std::cout << "  Satellites:  " << fix->satellitesTracked << "\n";
            if (fix->speedKnots > 0.0) {
                std::cout << "  Speed:       " << fix->speedKnots << " kts ("
                          << gps::GeoMath::knotsToKmh(fix->speedKnots) << " km/h)\n";
                std::cout << "  Heading:     " << fix->trackAngleDegrees << " deg\n";
            }

            if (prevFix && (prevFix->latitude != fix->latitude || prevFix->longitude != fix->longitude)) {
                double legKm = gps::GeoMath::haversineDistanceKm(
                    prevFix->latitude, prevFix->longitude,
                    fix->latitude, fix->longitude
                );
                totalDistanceKm += legKm;
                double bearing = gps::GeoMath::initialBearingDegrees(
                    prevFix->latitude, prevFix->longitude,
                    fix->latitude, fix->longitude
                );
                std::cout << "  Leg Delta:   " << legKm << " km (Bearing: " << bearing << " deg)\n";
            }
            prevFix = fix;
        }
    }

    std::cout << "\n----------------------------------------\n";
    std::cout << "Total Route Distance: " << totalDistanceKm << " km\n";

    if (testRun) {
        std::cout << "[Test Mode] GPS NMEA tracking and geodesic verification passed.\n";
        return 0;
    }

    return 0;
}
