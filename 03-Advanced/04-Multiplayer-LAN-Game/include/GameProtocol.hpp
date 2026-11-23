#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <cstdint>

namespace lan_game {

struct PlayerState {
    int id{0};
    std::string name;
    float x{0.0f};
    float y{0.0f};
    int health{100};
    int score{0};
};

struct PlayerInput {
    int sequence{0};
    float moveX{0.0f}; // -1.0 to +1.0
    float moveY{0.0f};
};

struct WorldSnapshot {
    uint32_t tick{0};
    std::vector<PlayerState> players;

    [[nodiscard]] std::string serialize() const {
        std::ostringstream oss;
        oss << "SNAPSHOT " << tick << " " << players.size();
        for (const auto& p : players) {
            oss << " " << p.id << " " << p.name << " " << p.x << " " << p.y
                << " " << p.health << " " << p.score;
        }
        oss << "\n";
        return oss.str();
    }

    static std::optional<WorldSnapshot> deserialize(const std::string& raw) {
        std::istringstream iss(raw);
        std::string tag;
        if (!(iss >> tag) || tag != "SNAPSHOT") return std::nullopt;

        WorldSnapshot snap;
        size_t count = 0;
        if (!(iss >> snap.tick >> count)) return std::nullopt;

        for (size_t i = 0; i < count; ++i) {
            PlayerState p;
            if (iss >> p.id >> p.name >> p.x >> p.y >> p.health >> p.score) {
                snap.players.push_back(p);
            } else {
                return std::nullopt;
            }
        }
        return snap;
    }
};

} // namespace lan_game
