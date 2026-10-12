#ifndef GAME_H
#define GAME_H

#include "board.h"

#define GAME_SUCCESS       0
#define GAME_ERR_NULL     -1
#define GAME_ERR_OVER     -2
#define GAME_ERR_BOUNDS   -3
#define GAME_ERR_OCCUPIED -4

typedef enum {
    PLAYER_X = 1,
    PLAYER_O = 2
} player_t;

typedef enum {
    IN_PROGRESS = 0,
    X_WINS,
    O_WINS
} game_state_t;

typedef struct {
    board_t board;
    player_t current_player;
    game_state_t state;
    int moves_made;
} game_t;

int game_init(game_t *game);
int make_move(game_t *game, int r, int c);

#endif