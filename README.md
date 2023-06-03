# MCTS-Leaf-Parallelization-Hex

A Monte Carlo Tree Search engine with Leaf Parallelization using standard C++ 14 libraries adapted for the game of Hex with a variable board size, with an additional Logger class, allowing for the visualization of the decision making process of the algorithm.

The implementation is adaptable for other games, for which a different Board class should be implemented. Here, the winner is detected using recursive Depth First Search - in some scenarios, Union Find might be a better choice.
