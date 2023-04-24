#include <iostream>
#include <chrono>
#include "game.h"

int main() {
	int board_size = 5;
	double exploration_constant = std::sqrt(2.0); // Adjust this value as needed
	std::chrono::milliseconds move_time_limit(15000); // Adjust this value as needed

	char human_player;
	std::cout << "Choose your player color (B/R): ";
	std::cin >> human_player;
	while (human_player != 'B' && human_player != 'R') {
		std::cout << "Invalid input! Choose your player color (B/R): ";
		std::cin >> human_player;
	}
	Game game(board_size, exploration_constant, move_time_limit, human_player);
	game.play_game();

	return 0;
}