# Hex Board Game with Monte Carlo Tree Search Agent in C++14

## Overview
This repository contains an implementation of the board game of [Hex](https://en.wikipedia.org/wiki/Hex_(board_game)) with an agent based on [Monte Carlo Tree Search](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) (MCTS) utilizing optional [leaf parallelization](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#:~:text=Leaf%20parallelization) in a console interface. The MCTS agent has configurable hyperparameters, and the size of the board is variable. Standard C++14 libraries are used.

![img1](./images/1.jpg)

## Features
The application includes several modes of gameplay:

- **Human vs Robot:** A user competes against the AI.
- **Robot vs Robot:** Two AI agents compete, allowing for evaluation and comparison of different hyperparameters.
- **Human vs Human:** Two users compete, each taking turns on the same console.

For those interested in studying MCTS, the application provides optional logging functionality. When operated in single-threaded mode, the agent is capable of producing a detailed log, showing the decision-making process behind each move across MCTS iterations.

![img2](./images/2.jpg)

## Structure

- `Cell_state`: An enum that symbolizes the state of a Hex game board cell - it could be vacant or claimed by a player (Blue or Red).
- `Board`: represents the Hex game board, providing functionality for its initialization, move validation, game state representation, determining the game outcome using recursive [depth-first search](https://en.wikipedia.org/wiki/Depth-first_search), and visualization.
- `Mcts_agent`: Implements the MCTS algorithm simulating gameplay to select the most promising move, supporting optional thread-safe parallelization and detailed logging. The nested class `Node` symbolizes a game tree node.
- `Logger`: A thread-safe singleton class for logging operations and state changes within the MCTS algorithm. It is used as a member class of `Mcts_agent`.
- `Player`: An abstract base class that outlines the necessary structure and methods for any player type in a game, with `Human_player` and `Mcts_player` as concrete subclasses representing a human player and a player that uses MCTS.
- `Game`: Encapsulates a complete Hex game, handling the game loop, player turns, board management, and state transitions for two players.
- `console_interface`: A suite of functions that provide an interactive console interface for users to set up and play different configurations of the Hex game, handle user input validation, manage game parameters, and display relevant game information.
- `main`: invokes the `run_console_interface` function.

Refer to the corresponding header files for detailed documentation.

## Building
The recommended method to build the project is via Visual Studio with the C++ Desktop Development environment installed. Once installed, follow these steps:

1. Click the green 'Code' button at the top of this page.
2. Select 'Open with Visual Studio'.
3. In the Solution Explorer, select the `CMakeLists.txt` file  and build the project (F5).

Additionally, a `Makefile` is available for use. 

Contributions to this project are welcome. Happy coding!
