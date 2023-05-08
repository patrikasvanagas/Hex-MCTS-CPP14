#pragma once
#include "abstract_board.h"
#include "abstract_player.h"

class AbstractGame {
public:
    virtual void play() = 0;
    virtual ~AbstractGame() = default;
};
