#ifndef CELL_STATE_H
#define CELL_STATE_H

#include <ostream>

/**
 * @enum Cell_state
 * @brief Represents the state of a cell in a Hex game.
 *
 * This enumeration class is used to denote the state of individual cells
 * on the Hex game board. A cell in a Hex game can be in one of three states:
 * Empty, Blue, or Red.
 *
 * Enumeration values:
 * @value Empty The cell has not been claimed by any player.
 * @value Blue The cell has been claimed by the Blue player.
 * @value Red The cell has been claimed by the Red player.
 *
 * This enum is vital in operations like board visualization, gameplay mechanics
 * (e.g., claiming cells), and in determining the winner of the game.
 */
enum class Cell_state {
  Empty,  ///< The cell is not claimed by any player.
  Blue,   ///< The cell is claimed by the Blue player.
  Red     ///< The cell is claimed by the Red player.
};

/**
 * @brief Overloaded stream insertion operator for Cell_state.
 *
 * This function implements the overloaded stream insertion operator for the
 * Cell_state enumeration class. It transforms the state of a cell into a single
 * character that can be directly inserted into an output stream:
 * - 'Empty' state is translated into '.'
 * - 'Blue' state is translated into 'B'
 * - 'Red' state is translated into 'R'
 *
 * @param os The output stream where the state will be written.
 * @param state The Cell_state that needs to be translated into a character and
 * written into the stream.
 * @return A reference to the output stream with the inserted state character.
 */
std::ostream& operator<<(std::ostream& os, const Cell_state& state);

#endif  // CELL_STATE_H