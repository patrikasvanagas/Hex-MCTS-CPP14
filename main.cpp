#include <chrono>
#include "game.h"


int main() {
    Human_player human;
    Mcts_player mcts_agent_1(1.41, std::chrono::milliseconds(100), false, true);
    Mcts_player mcts_agent_2(1.41, std::chrono::milliseconds(2000), false, false);
    Game game(2, &mcts_agent_1, &human);
    game.play();
	return 0;
}

