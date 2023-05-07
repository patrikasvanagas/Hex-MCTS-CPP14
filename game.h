#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "mcts_agent.h"
#include <chrono>

class Game {
public:
    Game(int size, double exploration_constant, std::chrono::milliseconds move_time_limit, char human_player);
    void play_game();

private:
    Board board;
    char current_player;
    char human_player;
    double exploration_constant;
    std::chrono::milliseconds move_time_limit;

    void switch_player();
    void make_agent_move();
    void make_human_move();
};
#endif