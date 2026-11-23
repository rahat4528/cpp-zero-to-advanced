#pragma once

#include <string>
#include <optional>
#include <cstdint>

namespace gps {

struct GPSFix {
    std::string utcTime;
    double latitude{0.0};
    double longitude{0.0};
    double altitudeMeters{0.0};
    int fixQuality{0};
    int satellitesTracked{0};
    double speedKnots{0.0};
    double trackAngleDegrees{0.0};
    std::string date;
    bool isValid{false};
};

class NMEAParser {
public:
    [[nodiscard]] static bool validateChecksum(const std::string& sentence);
    [[nodiscard]] static double convertNMEACoordToDecimal(const std::string& rawCoord, char direction);
    [[nodiscard]] static std::optional<GPSFix> parseSentence(const std::string& sentence);
};

} // namespace gps
