#pragma once

#include <vector>
#include <string>
#include <optional>

namespace tictactoe {

enum class Player {
    None = 0,
    X,
    O
};

enum class GameState {
    InProgress,
    X_Wins,
    O_Wins,
    Draw
};

class TicTacToeBoard {
public:
    TicTacToeBoard();

    void reset();
    bool makeMove(int row, int col, Player p);
    [[nodiscard]] Player getCell(int row, int col) const;
    [[nodiscard]] bool isCellEmpty(int row, int col) const;
    [[nodiscard]] GameState evaluateState() const;
    [[nodiscard]] bool isGameOver() const;
    [[nodiscard]] int getMoveCount() const;

    [[nodiscard]] std::string toString() const;
    static char playerToChar(Player p);
    static Player charToPlayer(char c);

private:
    Player m_cells[3][3];
    int m_moves{0};
};

} // namespace tictactoe
