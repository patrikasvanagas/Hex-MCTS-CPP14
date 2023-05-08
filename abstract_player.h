#pragma once
#include "abstract_board.h"

class AbstractPlayer {
public:
    virtual std::pair<int, int> choose_move(const AbstractBoard& board, char player) = 0;
    virtual ~AbstractPlayer() = default;
};
