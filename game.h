#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "mcts_agent.h"

class Game {
public:
    Game(int size);
    void play_game();

private:
    Board board;
    char current_player;

    void switch_player();
    void make_agent_move();
    void make_human_move();
};

#endif
