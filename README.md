# C Maze Generator and Solver

## Overview

This project is a command-line application written in C that procedurally generates a random maze and finds the shortest path from a starting point to an end point. The program takes user-defined dimensions, creates a maze using a Depth-First Search (DFS) algorithm, and then solves it by modeling the maze as a graph and applying Dijkstra's algorithm to find the shortest path.

The application is designed to demonstrate core concepts in algorithmics, including graph theory, pathfinding, and dynamic memory management in C.

---

## Features

* **Random Maze Generation**: Creates a perfect maze (no loops) of any valid user-specified odd dimensions using a recursive Depth-First Search (DFS) algorithm.
* **Graph-Based Solving**: Converts the generated maze into a graph data structure where each path cell is a vertex and connections between them are edges.
* **Shortest Path Finding**: Implements Dijkstra's algorithm to efficiently find the shortest path from a randomly selected start point to an end point.
* **ASCII Visualization**: Renders the maze, the start/end points, and the final solved path directly in the console using ASCII characters.
* **Input Validation**: Ensures that the user provides valid dimensions (positive, odd integers within a specific range).

---

## Algorithms Used

* **Maze Generation**: Depth-First Search (DFS) is used to carve paths through a grid, ensuring that every cell is reachable and creating a maze without any closed loops.
* **Pathfinding**: The maze is treated as an unweighted graph, and Dijkstra's algorithm is applied to find the single-source shortest path from the start vertex to the end vertex.

---

## How to Compile and Run

### Prerequisites

* A C compiler (e.g., `gcc`, `clang`).

### Compilation

The program can be compiled using a standard C compiler. It is recommended to use the `-Wall` and `-pedantic` flags to ensure code quality and standard compliance.

```bash
gcc -Wall -pedantic mini_projekt_3.c -o maze_solver
