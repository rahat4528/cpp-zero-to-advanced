#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "TicTacToeBoard.hpp"
#include "GameProtocol.hpp"

TEST_CASE("TicTacToeBoard - Move Validation and Win Conditions") {
    tictactoe::TicTacToeBoard board;

    SUBCASE("Empty Board Initial State") {
        CHECK(board.getMoveCount() == 0);
        CHECK(board.isGameOver() == false);
        CHECK(board.evaluateState() == tictactoe::GameState::InProgress);
        CHECK(board.isCellEmpty(1, 1) == true);
    }

    SUBCASE("Row Win Condition") {
        CHECK(board.makeMove(1, 0, tictactoe::Player::X) == true);
        CHECK(board.makeMove(1, 1, tictactoe::Player::X) == true);
        CHECK(board.makeMove(1, 2, tictactoe::Player::X) == true);
        CHECK(board.evaluateState() == tictactoe::GameState::X_Wins);
        CHECK(board.isGameOver() == true);
    }

    SUBCASE("Column Win Condition") {
        CHECK(board.makeMove(0, 2, tictactoe::Player::O) == true);
        CHECK(board.makeMove(1, 2, tictactoe::Player::O) == true);
        CHECK(board.makeMove(2, 2, tictactoe::Player::O) == true);
        CHECK(board.evaluateState() == tictactoe::GameState::O_Wins);
    }

    SUBCASE("Diagonal Win Condition") {
        CHECK(board.makeMove(0, 0, tictactoe::Player::X) == true);
        CHECK(board.makeMove(1, 1, tictactoe::Player::X) == true);
        CHECK(board.makeMove(2, 2, tictactoe::Player::X) == true);
        CHECK(board.evaluateState() == tictactoe::GameState::X_Wins);
    }

    SUBCASE("Duplicate Move Rejection") {
        CHECK(board.makeMove(0, 0, tictactoe::Player::X) == true);
        CHECK(board.makeMove(0, 0, tictactoe::Player::O) == false);
        CHECK(board.getCell(0, 0) == tictactoe::Player::X);
    }

    SUBCASE("Draw Evaluation") {
        // X O X
        // X X O
        // O X O
        board.makeMove(0, 0, tictactoe::Player::X);
        board.makeMove(0, 1, tictactoe::Player::O);
        board.makeMove(0, 2, tictactoe::Player::X);

        board.makeMove(1, 0, tictactoe::Player::X);
        board.makeMove(1, 1, tictactoe::Player::X);
        board.makeMove(1, 2, tictactoe::Player::O);

        board.makeMove(2, 0, tictactoe::Player::O);
        board.makeMove(2, 1, tictactoe::Player::X);
        board.makeMove(2, 2, tictactoe::Player::O);

        CHECK(board.evaluateState() == tictactoe::GameState::Draw);
        CHECK(board.isGameOver() == true);
    }
}

TEST_CASE("GameProtocol - Packet Serialization") {
    using namespace tictactoe;

    SUBCASE("MOVE Packet") {
        NetworkPacket p{"MOVE", 1, 2, 'X', ""};
        std::string raw = p.serialize();
        CHECK(raw == "MOVE 1 2 X\n");

        auto opt = NetworkPacket::deserialize(raw);
        REQUIRE(opt.has_value());
        CHECK(opt->command == "MOVE");
        CHECK(opt->row == 1);
        CHECK(opt->col == 2);
        CHECK(opt->player == 'X');
    }

    SUBCASE("START and QUIT Packets") {
        NetworkPacket pStart{"START", -1, -1, 'O', ""};
        auto optStart = NetworkPacket::deserialize(pStart.serialize());
        REQUIRE(optStart.has_value());
        CHECK(optStart->player == 'O');

        NetworkPacket pQuit{"QUIT", -1, -1, '\0', ""};
        auto optQuit = NetworkPacket::deserialize(pQuit.serialize());
        REQUIRE(optQuit.has_value());
        CHECK(optQuit->command == "QUIT");
    }
}
