#include <iostream>
#include "game.h"
#include "mcts_agent.h"

Game::Game(int size, double exploration_constant, std::chrono::milliseconds move_time_limit, char human_player)
	: board(size), current_player('B'), exploration_constant(exploration_constant), move_time_limit(move_time_limit), human_player(human_player) {}

void Game::switch_player() {
	current_player = (current_player == 'B') ? 'R' : 'B';
}

void Game::make_agent_move() {
	MCTSAgent agent(exploration_constant, move_time_limit);
	std::pair<int, int> move = agent.choose_move(board, current_player);
	board.make_move(move.first, move.second, current_player);
}

void Game::make_human_move() {
	int x;
	char col_char;
	bool valid_move = false;

	while (!valid_move) {
		std::cout << "Enter the row as a number and the column as a letter separated by space: ";
		std::cin >> x >> col_char;

		if (col_char >= 'a' && col_char < 'a' + board.get_board_size()) {
			int y = col_char - 'a';
			try {
				board.make_move(x - 1, y, current_player);
				valid_move = true;
			}
			catch (const std::invalid_argument&) {
				std::cout << "Invalid move! Try again." << std::endl;
			}
		}
		else {
			std::cout << "Invalid column! Try again." << std::endl;
		}
	}
}

void Game::play_game() {
	char winner = '.';
	while (winner == '.') {
		board.display_board();
		if (current_player == human_player) {
			make_human_move();
		}
		else {
			make_agent_move();
		}
		winner = board.check_winner();
		switch_player();
	}
	board.display_board();
	std::cout << "Player " << winner << " wins!" << std::endl;
}