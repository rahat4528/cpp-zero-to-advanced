#include "GPSSimulator.hpp"
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace gps {

std::string GPSSimulator::createNMEASentence(const std::string& body) {
    uint8_t checksum = 0;
    for (char c : body) {
        checksum ^= static_cast<uint8_t>(c);
    }
    std::ostringstream oss;
    oss << "$" << body << "*" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(checksum);
    return oss.str();
}

std::vector<std::string> GPSSimulator::generateRouteSentences() {
    std::vector<std::string> list;

    // Waypoint 1: London (51 deg 30.264 min N, 000 deg 07.668 min W)
    list.push_back(createNMEASentence("GPGGA,120000.00,5130.264,N,00007.668,W,1,08,1.0,25.4,M,0.0,M,,"));
    list.push_back(createNMEASentence("GPRMC,120000.00,A,5130.264,N,00007.668,W,120.5,145.2,231126,,,A"));

    // Waypoint 2: Mid-channel
    list.push_back(createNMEASentence("GPGGA,121500.00,5030.000,N,00100.000,E,1,09,0.9,150.0,M,0.0,M,,"));
    list.push_back(createNMEASentence("GPRMC,121500.00,A,5030.000,N,00100.000,E,245.0,145.0,231126,,,A"));

    // Waypoint 3: Approaching Paris (48 deg 51.396 min N, 002 deg 21.132 min E)
    list.push_back(createNMEASentence("GPGGA,123000.00,4851.396,N,00221.132,E,1,10,0.8,35.0,M,0.0,M,,"));
    list.push_back(createNMEASentence("GPRMC,123000.00,A,4851.396,N,00221.132,E,110.0,144.8,231126,,,A"));

    return list;
}

} // namespace gps
