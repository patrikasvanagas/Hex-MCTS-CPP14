#include "game.h"

#include <iostream>

/**
 * @class Game
 * @brief Represents a Hex game.
 *
 * This class encapsulates a complete Hex game, including the game board and the
 * two players. It handles the game loop, player turns, and game state
 * transitions.
 */

/**
 * @brief Constructs a new Game object.
 *
 * This constructor initializes a new game with a specified board size and two
 * unique players. Players are moved into the Game object, ensuring that the
 * Game has exclusive ownership of the players.
 *
 * @param board_size The size of the game board.
 * @param player1 Unique pointer to the first player.
 * @param player2 Unique pointer to the second player.
 */
Game::Game(int board_size, std::unique_ptr<Player> player1,
           std::unique_ptr<Player> player_2)
    : board(board_size), current_player_index(0) 
{
  players[0] = std::move(player1);
  players[1] = std::move(player_2);
}

/**
 * @brief Switches the current player.
 *
 * This function switches the turn to the other player. It is called after a
 * player has made a move.
 */
void Game::switch_player() 
{ 
    current_player_index = 1 - current_player_index; 
}

/**
 * @brief Starts and manages the Hex game.
 *
 * This function contains the main game loop. It continues until a player wins,
 * i.e., when the board's check_winner() function no longer returns
 * Cell_state::Empty. On each iteration of the loop, it:
 *   - Displays the current player's turn,
 *   - Displays the current state of the board,
 *   - Asks the current player to choose a move,
 *   - Makes the chosen move on the board,
 *   - Switches to the other player.
 * Once a player wins, it displays the final state of the board and the winner.
 */
void Game::play() 
{
  while (board.check_winner() == Cell_state::Empty) {
    Cell_state current_player =
        current_player_index == 0 ? Cell_state::Blue : Cell_state::Red;
    std::cout << "\nPlayer " << current_player << "'s turn:" << std::endl;
    board.display_board();
    std::pair<int, int> chosen_move =
        players[current_player_index]->choose_move(board, current_player);
    int chosen_row = chosen_move.first + 1;
    char chosen_col = chosen_move.second + 'a';
    std::cout << "\nPlayer " << current_player << " chose move: " << chosen_row
              << " " << chosen_col << std::endl;
    board.make_move(chosen_move.first, chosen_move.second, current_player);
    switch_player();
  }
  board.display_board();
  Cell_state winning_player =
      (current_player_index == 0) ? Cell_state::Red : Cell_state::Blue;
  std::cout << "Player " << winning_player << " wins!" << std::endl;
}
