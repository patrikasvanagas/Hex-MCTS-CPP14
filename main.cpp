#include <iostream>
#include <chrono>
#include "game.h"


int main() {

    HumanPlayer human;
    MCTSPlayer mcts_agent_1(1.41, std::chrono::milliseconds(100), false, true);
    MCTSPlayer mcts_agent_2(1.41, std::chrono::milliseconds(10000), false);
    Game game(3, &mcts_agent_1, &human);
    game.play();

    //test_winning_condition();

	return 0;
}



