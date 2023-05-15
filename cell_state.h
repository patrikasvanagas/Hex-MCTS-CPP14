#ifndef CELL_STATE_H
#define CELL_STATE_H

#include <ostream>

enum class Cell_state 
{
	Empty, Blue, Red 
};

// Overload the stream insertion operator for Cell_state
std::ostream& operator<<(std::ostream& os, const Cell_state& state);

#endif  // CELL_STATE_H