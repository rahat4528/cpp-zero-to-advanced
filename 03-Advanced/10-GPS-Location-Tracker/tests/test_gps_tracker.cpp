#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "NMEAParser.hpp"
#include "GeoMath.hpp"
#include "GPSSimulator.hpp"

using namespace gps;

TEST_SUITE("GPS Location Tracker and Geodesic Calculations") {
    TEST_CASE("NMEA Checksum Calculation and Validation") {
        std::string validSentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
        CHECK(NMEAParser::validateChecksum(validSentence));

        std::string invalidChecksum = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF";
        CHECK_FALSE(NMEAParser::validateChecksum(invalidChecksum));

        std::string missingDollar = "GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
        CHECK_FALSE(NMEAParser::validateChecksum(missingDollar));
    }

    TEST_CASE("NMEA Coordinate Conversion to Decimal Degrees") {
        // 51 deg 30.264 min N -> 51 + 30.264/60 = 51.5044
        double lat = NMEAParser::convertNMEACoordToDecimal("5130.264", 'N');
        CHECK(lat == doctest::Approx(51.5044));

        // 000 deg 07.668 min W -> -(0 + 7.668/60) = -0.1278
        double lon = NMEAParser::convertNMEACoordToDecimal("00007.668", 'W');
        CHECK(lon == doctest::Approx(-0.1278));
    }

    TEST_CASE("GPGGA and GPRMC Sentence Parsing") {
        std::string sentence = GPSSimulator::createNMEASentence("GPGGA,092750.00,5321.6802,N,00630.3372,W,1,08,1.03,61.7,M,55.2,M,,");
        auto fix = NMEAParser::parseSentence(sentence);
        REQUIRE(fix.has_value());
        CHECK(fix->isValid);
        CHECK(fix->satellitesTracked == 8);
        CHECK(fix->altitudeMeters == doctest::Approx(61.7));
    }

    TEST_CASE("Haversine Distance and Bearing Calculations") {
        // London (51.5074 N, -0.1278 W) to Paris (48.8566 N, 2.3522 E) ~ 343 km
        double distance = GeoMath::haversineDistanceKm(51.5074, -0.1278, 48.8566, 2.3522);
        CHECK(distance > 340.0);
        CHECK(distance < 350.0);

        double bearing = GeoMath::initialBearingDegrees(51.5074, -0.1278, 48.8566, 2.3522);
        CHECK(bearing > 140.0);
        CHECK(bearing < 160.0);
    }
}
