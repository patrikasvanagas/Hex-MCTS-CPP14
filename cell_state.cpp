#include "cell_state.h"

std::ostream& operator<<(std::ostream& os, const Cell_state& state) {
    switch (state) {
    case Cell_state::Empty: os << '.'; break;
    case Cell_state::Blue:  os << 'B'; break;
    case Cell_state::Red:   os << 'R'; break;
    }
    return os;
}
