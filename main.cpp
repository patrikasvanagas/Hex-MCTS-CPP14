// #include <chrono>
#include "console_interface.h"
#include "game.h"

int main() {
  run_console_interface();
  // auto human_player = std::make_unique<Human_player>();
  // auto mcts_agent_player = std::make_unique<Mcts_player>(1.41,
  // std::chrono::milliseconds(100), true, false); auto
  // parallelized_mcts_agent_player = std::make_unique<Mcts_player>(1.41,
  // std::chrono::milliseconds(30000), true, false); Game game(2,
  // std::move(mcts_agent_player), std::move(human_player)); game.play();
  return 0;
}
