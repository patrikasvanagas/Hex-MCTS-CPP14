#ifndef PLAYER_H
#define PLAYER_H

#include <utility>
#include <chrono>
#include "hex_board.h"

class Player {
public:
    virtual std::pair<int, int> choose_move(const Board& board, char player) = 0;
};

class HumanPlayer : public Player {
public:
    std::pair<int, int> choose_move(const Board& board, char player) override;
};

class MCTSPlayer : public Player {
public:
    MCTSPlayer(double exploration_constant, std::chrono::milliseconds move_time_limit);
    std::pair<int, int> choose_move(const Board& board, char player) override;

private:
    double exploration_constant;
    std::chrono::milliseconds move_time_limit;
};

#endif
