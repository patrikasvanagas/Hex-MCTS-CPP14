#include <chrono>
#include "game.h"


int main() {
    Human_player human;
    Mcts_player mcts_agent(1.41, std::chrono::milliseconds(5000), false, true);
    Mcts_player parallelized_mcts_agent(1.41, std::chrono::milliseconds(30000), true, false);
    Game game(3, &mcts_agent, &human);
    game.play();
	return 0;
}

