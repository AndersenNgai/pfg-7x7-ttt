# pfg-7x7-ttt

The purpose of this project is to determine whether player X or O has a winning strategy in my version of Tic Tac Toe.
The rules are as follows:
* The board is (7 x 7).
* Player X goes first, then Player O.
* A player wins immediately by getting 4 in a row. However, getting 3 in a row results in them immediately losing. Getting both at the same time still results in an immediate win.
* If the board is full and no one has won, Player X loses by default. This counters the first player advantage.

The game logic is finished, and I will begin to determine whether Player X or O (or neither player) has a definitive winning strategy. In the meantime, you can play the game by running `make` and then `bin/ttt`. Note that you need GCC and Makefile installed before you can do this.

## Directory Structure

```text
pfg-7x7-ttt/
├── include/
│   └── game.h
├── src/
│   ├── game.c
│   └── main.c
├── tests/
│   └── game_tests.c
├── bin/                 # Created by running `make`
├── .gitignore
└── Makefile```
