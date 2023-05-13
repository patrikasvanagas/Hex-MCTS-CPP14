#include <chrono>
#include "game.h"


int main() {

    HumanPlayer human;
    MCTSPlayer mcts_agent_1(1.41, std::chrono::milliseconds(200), false, true);
    MCTSPlayer mcts_agent_2(1.41, std::chrono::milliseconds(2000), false, false);
    Game game(3, &mcts_agent_1, &mcts_agent_2);
    game.play();

	return 0;
}

