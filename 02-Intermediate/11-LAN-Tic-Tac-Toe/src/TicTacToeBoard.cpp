#include "TicTacToeBoard.hpp"
#include <sstream>

namespace tictactoe {

TicTacToeBoard::TicTacToeBoard() {
    reset();
}

void TicTacToeBoard::reset() {
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            m_cells[r][c] = Player::None;
        }
    }
    m_moves = 0;
}

bool TicTacToeBoard::makeMove(int row, int col, Player p) {
    if (row < 0 || row >= 3 || col < 0 || col >= 3) return false;
    if (m_cells[row][col] != Player::None) return false;
    if (p == Player::None) return false;

    m_cells[row][col] = p;
    m_moves++;
    return true;
}

Player TicTacToeBoard::getCell(int row, int col) const {
    if (row < 0 || row >= 3 || col < 0 || col >= 3) return Player::None;
    return m_cells[row][col];
}

bool TicTacToeBoard::isCellEmpty(int row, int col) const {
    return getCell(row, col) == Player::None;
}

GameState TicTacToeBoard::evaluateState() const {
    // Check rows
    for (int r = 0; r < 3; ++r) {
        if (m_cells[r][0] != Player::None &&
            m_cells[r][0] == m_cells[r][1] &&
            m_cells[r][1] == m_cells[r][2]) {
            return (m_cells[r][0] == Player::X) ? GameState::X_Wins : GameState::O_Wins;
        }
    }

    // Check cols
    for (int c = 0; c < 3; ++c) {
        if (m_cells[0][c] != Player::None &&
            m_cells[0][c] == m_cells[1][c] &&
            m_cells[1][c] == m_cells[2][c]) {
            return (m_cells[0][c] == Player::X) ? GameState::X_Wins : GameState::O_Wins;
        }
    }

    // Check diagonals
    if (m_cells[0][0] != Player::None &&
        m_cells[0][0] == m_cells[1][1] &&
        m_cells[1][1] == m_cells[2][2]) {
        return (m_cells[0][0] == Player::X) ? GameState::X_Wins : GameState::O_Wins;
    }

    if (m_cells[0][2] != Player::None &&
        m_cells[0][2] == m_cells[1][1] &&
        m_cells[1][1] == m_cells[2][0]) {
        return (m_cells[0][2] == Player::X) ? GameState::X_Wins : GameState::O_Wins;
    }

    if (m_moves >= 9) {
        return GameState::Draw;
    }

    return GameState::InProgress;
}

bool TicTacToeBoard::isGameOver() const {
    return evaluateState() != GameState::InProgress;
}

int TicTacToeBoard::getMoveCount() const {
    return m_moves;
}

std::string TicTacToeBoard::toString() const {
    std::ostringstream oss;
    oss << "\n  0   1   2\n";
    for (int r = 0; r < 3; ++r) {
        oss << r << " ";
        for (int c = 0; c < 3; ++c) {
            oss << playerToChar(m_cells[r][c]);
            if (c < 2) oss << " | ";
        }
        oss << "\n";
        if (r < 2) oss << "  ---+---+---\n";
    }
    return oss.str();
}

char TicTacToeBoard::playerToChar(Player p) {
    switch (p) {
        case Player::X: return 'X';
        case Player::O: return 'O';
        default: return ' ';
    }
}

Player TicTacToeBoard::charToPlayer(char c) {
    if (c == 'X' || c == 'x') return Player::X;
    if (c == 'O' || c == 'o') return Player::O;
    return Player::None;
}

} // namespace tictactoe
