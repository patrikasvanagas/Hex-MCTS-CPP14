#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"

class Game {
public:
    Game(int board_size , Player* player_1, Player* player_2);
    void play();

private:
    Board board;
    Player* players[2];
    int current_player_index;
    void switch_player();
};

#endif
