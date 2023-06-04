#ifndef GAME_H
#define GAME_H

#include <memory>

#include "board.h"
#include "cell_state.h"
#include "player.h"

/**
 * @class Game
 * @brief Represents a Hex game.
 *
 * This class encapsulates a complete Hex game, including the game board and the
 * two players. It handles the game loop, player turns, and game state
 * transitions.
 */
class Game {
 public:
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
  Game(int board_size, std::unique_ptr<Player> player_1,
       std::unique_ptr<Player> player_2);

  /**
   * @brief Starts and manages the Hex game.
   *
   * This function contains the main game loop. It continues until a player
   * wins, i.e., when the board's check_winner() function no longer returns
   * Cell_state::Empty.
   */
  void play();

 private:
  Board board;  ///< The Hex game board.
  std::unique_ptr<Player>
      players[2];            ///< Array of unique pointers to the two players.
  int current_player_index;  ///< Index of the current player.

  /**
   * @brief Switches the current player.
   *
   * This private function switches the turn to the other player. It is called
   * after a player has made a move.
   */
  void switch_player();
};

#endif