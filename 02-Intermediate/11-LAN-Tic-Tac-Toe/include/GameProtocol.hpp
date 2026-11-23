#pragma once

#include "TicTacToeBoard.hpp"
#include <string>
#include <sstream>
#include <optional>

namespace tictactoe {

struct NetworkPacket {
    std::string command; // "MOVE", "START", "QUIT", "MSG"
    int row{-1};
    int col{-1};
    char player{'\0'};
    std::string text;

    [[nodiscard]] std::string serialize() const {
        std::ostringstream oss;
        if (command == "MOVE") {
            oss << "MOVE " << row << " " << col << " " << player << "\n";
        } else if (command == "START") {
            oss << "START " << player << "\n";
        } else if (command == "QUIT") {
            oss << "QUIT\n";
        } else {
            oss << "MSG " << text << "\n";
        }
        return oss.str();
    }

    static std::optional<NetworkPacket> deserialize(const std::string& raw) {
        std::istringstream iss(raw);
        std::string cmd;
        if (!(iss >> cmd)) return std::nullopt;

        NetworkPacket pkt;
        pkt.command = cmd;
        if (cmd == "MOVE") {
            if (iss >> pkt.row >> pkt.col >> pkt.player) {
                return pkt;
            }
        } else if (cmd == "START") {
            if (iss >> pkt.player) {
                return pkt;
            }
        } else if (cmd == "QUIT") {
            return pkt;
        } else if (cmd == "MSG") {
            std::getline(iss >> std::ws, pkt.text);
            return pkt;
        }
        return std::nullopt;
    }
};

} // namespace tictactoe
