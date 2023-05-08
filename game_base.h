#ifndef GAME_BASE_H
#define GAME_BASE_H

#include "player.h"

template <typename TBoard>
class GameBase {
public:
    virtual void play() = 0;

protected:
    TBoard board;
    Player* players[2];
    int current_player_idx;

    virtual void switch_player() = 0;
};

#endif
