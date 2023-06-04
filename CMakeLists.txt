cmake_minimum_required(VERSION 3.10)

# Set the project name
project(mygame)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files
add_executable(Hex-MCTS
    main.cpp
    board.cpp
    cell_state.cpp
    console_interface.cpp
    game.cpp
    logger.cpp
    mcts_agent.cpp
    player.cpp
)
