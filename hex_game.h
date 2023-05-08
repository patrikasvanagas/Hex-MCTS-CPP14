#ifndef HEX_GAME_H
#define HEX_GAME_H

#include "hex_board.h"
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
