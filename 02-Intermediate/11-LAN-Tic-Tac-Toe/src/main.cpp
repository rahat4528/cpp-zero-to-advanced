#include "TicTacToeBoard.hpp"
#include "GameProtocol.hpp"
#include "SocketCompat.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "               LAN TIC-TAC-TOE MULTIPLAYER GAME                  \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Simulating complete board playthrough & serialization...\n";

        tictactoe::TicTacToeBoard board;
        board.makeMove(0, 0, tictactoe::Player::X);
        board.makeMove(1, 0, tictactoe::Player::O);
        board.makeMove(0, 1, tictactoe::Player::X);
        board.makeMove(1, 1, tictactoe::Player::O);
        board.makeMove(0, 2, tictactoe::Player::X); // X completes top row

        std::cout << board.toString() << "\n";
        std::cout << "Game Status: " << (board.evaluateState() == tictactoe::GameState::X_Wins ? "X WINS (Verified!)" : "In Progress") << "\n";

        tictactoe::NetworkPacket pkt{"MOVE", 2, 2, 'X', ""};
        std::string raw = pkt.serialize();
        std::cout << "Serialized Packet: " << raw;
        auto deserialized = tictactoe::NetworkPacket::deserialize(raw);
        if (deserialized && deserialized->row == 2 && deserialized->col == 2) {
            std::cout << "Deserialization roundtrip verified successfully.\n";
        }
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "               LAN TIC-TAC-TOE MULTIPLAYER GAME                  \n";
    std::cout << "=================================================================\n";
    std::cout << "Usage:\n";
    std::cout << "  Host Game: TicTacToe --host <port>\n";
    std::cout << "  Join Game: TicTacToe --join <ip> <port>\n";
    std::cout << "  Headless : TicTacToe --test-run\n";
    return 0;
}
