#include "NMEAParser.hpp"
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>

namespace gps {

static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool NMEAParser::validateChecksum(const std::string& sentence) {
    if (sentence.empty() || sentence.front() != '$') return false;

    size_t starPos = sentence.find('*');
    if (starPos == std::string::npos || starPos + 2 >= sentence.size()) return false;

    uint8_t calculated = 0;
    for (size_t i = 1; i < starPos; ++i) {
        calculated ^= static_cast<uint8_t>(sentence[i]);
    }

    std::string expectedHex = sentence.substr(starPos + 1, 2);
    int expected = 0;
    std::istringstream iss(expectedHex);
    if (!(iss >> std::hex >> expected)) return false;

    return calculated == static_cast<uint8_t>(expected);
}

double NMEAParser::convertNMEACoordToDecimal(const std::string& rawCoord, char direction) {
    if (rawCoord.empty()) return 0.0;

    double raw = std::stod(rawCoord);
    int degrees = static_cast<int>(raw / 100.0);
    double minutes = raw - (degrees * 100.0);
    double decimal = degrees + (minutes / 60.0);

    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    return decimal;
}

std::optional<GPSFix> NMEAParser::parseSentence(const std::string& sentence) {
    if (!validateChecksum(sentence)) return std::nullopt;

    size_t starPos = sentence.find('*');
    std::string body = sentence.substr(1, starPos - 1);
    auto tokens = split(body, ',');
    if (tokens.empty()) return std::nullopt;

    const std::string& tag = tokens[0];

    // $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx
    if (tag == "GPGGA" && tokens.size() >= 10) {
        GPSFix fix;
        fix.utcTime = tokens[1];
        if (!tokens[2].empty() && !tokens[3].empty()) {
            fix.latitude = convertNMEACoordToDecimal(tokens[2], tokens[3][0]);
        }
        if (!tokens[4].empty() && !tokens[5].empty()) {
            fix.longitude = convertNMEACoordToDecimal(tokens[4], tokens[5][0]);
        }
        fix.fixQuality = tokens[6].empty() ? 0 : std::stoi(tokens[6]);
        fix.satellitesTracked = tokens[7].empty() ? 0 : std::stoi(tokens[7]);
        fix.altitudeMeters = tokens[9].empty() ? 0.0 : std::stod(tokens[9]);
        fix.isValid = (fix.fixQuality > 0);
        return fix;
    }

    // $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,,,a
    if (tag == "GPRMC" && tokens.size() >= 10) {
        GPSFix fix;
        fix.utcTime = tokens[1];
        char status = tokens[2].empty() ? 'V' : tokens[2][0];
        fix.isValid = (status == 'A');

        if (!tokens[3].empty() && !tokens[4].empty()) {
            fix.latitude = convertNMEACoordToDecimal(tokens[3], tokens[4][0]);
        }
        if (!tokens[5].empty() && !tokens[6].empty()) {
            fix.longitude = convertNMEACoordToDecimal(tokens[5], tokens[6][0]);
        }
        fix.speedKnots = tokens[7].empty() ? 0.0 : std::stod(tokens[7]);
        fix.trackAngleDegrees = tokens[8].empty() ? 0.0 : std::stod(tokens[8]);
        fix.date = tokens[9];
        return fix;
    }

    return std::nullopt;
}

} // namespace gps
