#include "cell_state.h"

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
 * This makes it possible to print the state of cells in a human-readable
 * format, essential for debugging and game state visualization.
 *
 * @param os The output stream where the state will be written.
 * @param state The Cell_state that needs to be translated into a character and
 * written into the stream.
 * @return A reference to the output stream with the inserted state character.
 */
std::ostream& operator<<(std::ostream& os, const Cell_state& state) 
{
  switch (state) {
    case Cell_state::Empty:
      os << '.';
      break;
    case Cell_state::Blue:
      os << 'B';
      break;
    case Cell_state::Red:
      os << 'R';
      break;
  }
  return os;
}