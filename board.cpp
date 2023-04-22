#include <iostream>
#include "board.h"

Board::Board(int size) : board_size(size), board(size, std::vector<char>(size, '.')) {}

void Board::display_board() const {
    for (const auto &row : board) {
        for (const char &cell : row) {
            std::cout << cell << ' ';
        }
        std::cout << std::endl;
    }
}

bool Board::is_valid_move(int x, int y) const {
    return (x >= 0 && x < board_size) && (y >= 0 && y < board_size) && (board[x][y] == '.');
}

void Board::make_move(int x, int y, char player) {
    if (is_valid_move(x, y)) {
        board[x][y] = player;
    } else {
        throw std::invalid_argument("Invalid move!");
    }
}

bool Board::is_within_bounds(int x, int y) const {
    return x >= 0 && x < board_size && y >= 0 && y < board_size;
}

void Board::dfs(int x, int y, char player, std::vector<std::vector<bool>> &visited) const {
    static const std::vector<int> dx = {-1, 0, 1, 1, 0, -1};
    static const std::vector<int> dy = {-1, -1, 0, 1, 1, 0};

    visited[x][y] = true;

    for (int i = 0; i < 6; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (is_within_bounds(nx, ny) && !visited[nx][ny] && board[nx][ny] == player) {
            dfs(nx, ny, player, visited);
        }
    }
}

char Board::check_winner() const {
    char players[] = {'B', 'W'};
    for (char player : players) {
        std::vector<std::vector<bool>> visited(board_size, std::vector<bool>(board_size, false));
        if (player == 'B') {
            for (int y = 0; y < board_size; ++y) {
                if (board[0][y] == 'B' && !visited[0][y]) {
                    dfs(0, y, 'B', visited);
                }
            }

            for (int y = 0; y < board_size; ++y) {
                if (visited[board_size - 1][y]) {
                    return 'B';
                }
            }
        } else {
            for (int x = 0; x < board_size; ++x) {
                if (board[x][0] == 'W' && !visited[x][0]) {
                    dfs(x, 0, 'W', visited);
                }
            }

            for (int x = 0; x < board_size; ++x) {
                if (visited[x][board_size - 1]) {
                    return 'W';
                }
            }
        }
    }

    return '.';
}

int Board::get_board_size() const {
    return board_size;
}
