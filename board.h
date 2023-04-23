#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <cctype>
#include <vector>
#include <stdexcept>

class Board {
public:
    Board(int size);
    void display_board() const;
    bool is_valid_move(int x, int y) const;
    void make_move(int x, int y, char player);
    char check_winner() const;
    int get_board_size() const;

private:
    int board_size;
    std::vector<std::vector<char>> board;

    bool is_within_bounds(int x, int y) const;
    bool dfs(int x, int y, char player, std::vector<std::vector<bool>>& visited, bool& left_reached, bool& right_reached, bool& top_reached, bool& bottom_reached) const;
};

#endif
