#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"

class Game {
public:
    Game(int size, Player* player1, Player* player2);
    void play();

private:
    Board board;
    Player* players[2];
    int current_player_idx;
    void switch_player();
};

#endif
