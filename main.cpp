#include <iostream>
#include <chrono>
#include <cassert>
#include "hex_game.h"
#include "hex_board.h"
#include "mcts_agent.h"


int main() {

    HumanPlayer human;
    MCTSPlayer mcts_agent_1(1.41, std::chrono::milliseconds(300), true);
    MCTSPlayer mcts_agent_2(1.41, std::chrono::milliseconds(10000), false);
    Game game(3, &mcts_agent_1, &human);
    game.play();

    //test_winning_condition();

	return 0;
}


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

    Board board6(5);
    board6.make_move(3, 0, 'R');
    board6.make_move(3, 1, 'R');
    board6.make_move(2, 2, 'R');
    board6.make_move(1, 2, 'R');
    board6.make_move(1, 3, 'R');
    board6.make_move(1, 4, 'R');
    print_board_and_winner(board6);
}
