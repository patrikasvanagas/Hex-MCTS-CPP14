#include <iostream>
#include <chrono>
#include "game.h"
#include "board.h"


void print_board_and_winner(Board& board) {
    board.display_board();
    char winner = board.check_winner();
    std::cout << "Winner: " << winner << std::endl;
    std::cout << "------------------" << std::endl;
}

void test_winning_condition() {
    // Test case 1: 3x3 board, player 'B' wins
    Board board1(3);
    board1.make_move(0, 0, 'B');
    board1.make_move(1, 0, 'B');
    board1.make_move(2, 0, 'B');
    print_board_and_winner(board1);

    // Test case 2: 3x3 board, player 'R' wins
    Board board2(3);
    board2.make_move(0, 0, 'R');
    board2.make_move(0, 1, 'R');
    board2.make_move(0, 2, 'R');
    print_board_and_winner(board2);

    // Test case 3: 3x3 board, player 'B' wins
    Board board3(3);
    board3.make_move(0, 2, 'B');
    board3.make_move(1, 1, 'B');
    board3.make_move(2, 1, 'B');
    print_board_and_winner(board3);

    // Test case 4: 3x3 board, player 'R' wins
    Board board4(3);
    board4.make_move(1, 0, 'R');
    board4.make_move(1, 1, 'R');
    board4.make_move(0, 2, 'R');
    print_board_and_winner(board4);

    // Test case 5: 3x3 board, no winner
    Board board5(3);
    board5.make_move(0, 0, 'B');
    board5.make_move(1, 1, 'B');
    board5.make_move(2, 0, 'B');
    print_board_and_winner(board5);
}



int main() {


    test_winning_condition();


	//int board_size = 3;
	//double exploration_constant = std::sqrt(2.0); // Adjust this value as needed
	//std::chrono::milliseconds move_time_limit(1000); // Adjust this value as needed

	//char human_player = 'B';
	////std::cout << "Choose your player color (B/R): ";
	////std::cin >> human_player;
	//while (human_player != 'B' && human_player != 'R') {
	//	std::cout << "Invalid input! Choose your player color (B/R): ";
	//	std::cin >> human_player;
	//}
	//Game game(board_size, exploration_constant, move_time_limit, human_player);
	//game.play_game();

	return 0;
}

