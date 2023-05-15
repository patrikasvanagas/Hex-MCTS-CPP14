#ifndef CELL_STATE_H
#define CELL_STATE_H

#include <ostream>

/**
 * @enum Cell_state
 * @brief Represents the state of a cell in a Hex game.
 *
 * This enum class is used throughout the repo to denote the state of
 * individual cells on the game board. A cell in a Hex game can be in one of
 * three states: Empty, Blue, or Red. Empty indicates that no player has yet
 * claimed the cell. Blue and Red represent cells claimed by the respective
 * players. It is also used in the board and MCTS operations, e.g. detecting
 * the winer of the game.
 */
enum class Cell_state 
{
	Empty, Blue, Red 
};

/**
 * @brief Overload of the stream insertion operator for Cell_state.
 *
 * This overload allows instances of Cell_state to be directly inserted into an
 * output stream. It is used for printing the state of a cell in a
 * human-readable format.
 *
 * @param os The output stream.
 * @param state The state to be inserted into the stream.
 * @return A reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const Cell_state& state);

#endif  // CELL_STATE_H