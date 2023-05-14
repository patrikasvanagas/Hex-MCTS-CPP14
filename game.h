#ifndef GAME_H
#define GAME_H

#include <memory>

#include "board.h"
#include "cell_state.h"
#include "player.h"

class Game {
 public:
  Game(int board_size, std::unique_ptr<Player> player_1,
       std::unique_ptr<Player> player_2);
  void play();

 private:
  Board board;
  std::unique_ptr<Player> players[2];
  int current_player_index;
  void switch_player();
};

#endif