#pragma once
#include <vector>

class AbstractBoard {
public:
    virtual void display_board() const = 0;
    virtual bool is_valid_move(int x, int y) const = 0;
    virtual void make_move(int x, int y, char player) = 0;
    virtual int get_board_size() const = 0;
    virtual char check_winner() const = 0;

    virtual ~AbstractBoard() = default;
};
