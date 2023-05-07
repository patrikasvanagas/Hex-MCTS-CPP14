#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <cctype>
#include <vector>
#include <stdexcept>
#include <array>

class Board {
public:
    Board(int size);
    void display_board() const;
    bool is_valid_move(int x, int y) const;
    void make_move(int x, int y, char player);
    char check_winner();
    int get_board_size() const;

private:
    int board_size;
    std::vector<std::vector<char>> board;
    std::array<int, 6> dx = {-1, -1, 0, 1, 1, 0};
    std::array<int, 6> dy = {0, 1, 1, 0, -1, -1};

    bool is_within_bounds(int x, int y) const;
    bool dfs(int x, int y, int target_x, int target_y, char player);
    bool is_connected(int x1, int y1, int x2, int y2);
    bool is_valid(int x, int y);
};

#endif
