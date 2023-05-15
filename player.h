#ifndef PLAYER_H
#define PLAYER_H

#include <chrono>
#include <utility>

#include "board.h"

class Player 
{
 public:
  virtual std::pair<int, int> choose_move(const Board& board,
                                          Cell_state player) = 0;
};

class Human_player : public Player 
{
 public:
  std::pair<int, int> choose_move(const Board& board,
                                  Cell_state player) override;
};

class Mcts_player : public Player 
{
 public:
  Mcts_player(double exploration_factor,
              std::chrono::milliseconds max_decision_time,
              bool is_parallelized = false, bool is_verbose = false);
  std::pair<int, int> choose_move(const Board& board,
                                  Cell_state player) override;
  bool get_is_verbose() const;

 private:
  double exploration_factor;
  std::chrono::milliseconds max_decision_time;
  bool is_parallelized;
  bool is_verbose;
};

#endif