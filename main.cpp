#include <chrono>
#include "game.h"


int main() {
    Human_player human;
    Mcts_player mcts_agent(1.41, std::chrono::milliseconds(30000), false, false);
    Mcts_player parallelized_mcts_agent(1.41, std::chrono::milliseconds(30000), true, false);
    Game game(7, &mcts_agent, &parallelized_mcts_agent);
    game.play();
	return 0;
}

