CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++14

# List of source files
SRCS = main.cpp board.cpp cell_state.cpp console_interface.cpp game.cpp logger.cpp mcts_agent.cpp player.cpp
# List of object files
OBJS = $(SRCS:.cpp=.o)

# Name of the output binary
TARGET = mygame

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
