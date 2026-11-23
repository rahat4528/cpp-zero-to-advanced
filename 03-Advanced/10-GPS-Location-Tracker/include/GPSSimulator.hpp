#pragma once

#include <string>
#include <vector>

namespace gps {

class GPSSimulator {
public:
    [[nodiscard]] static std::string createNMEASentence(const std::string& body);
    [[nodiscard]] static std::vector<std::string> generateRouteSentences();
};

} // namespace gps
