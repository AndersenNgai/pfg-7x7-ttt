#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define BOARD_SIZE 7

/**
  * GAME STATE LAYOUT is very compact
  * game_state_t has 2 fields that are both uint64_t: player_x then player_o
  *
  * From rightmost bit to leftmost bit:
  * Bits 0-48 of player_x are for Player X's pieces
  * Bit 49 of player_x is the current player (0 = PLAYER_X, 1 = PLAYER_O)
  * Bits 50-51 of player_x are for the game's current state (0 = IN_PROGRESS, 1 = X_WINS, 2 = O_WINS)
  * Bits 52-57 are the number of previous moves made so far
*/
typedef struct {
    uint64_t player_x;
    uint64_t player_o;
} game_t;

// Bitmasks and extraction
#define BOARD_MASK         0x1FFFFFFFFFFFFULL // Bits 0-48
#define TURN_BIT           (1ULL << 49)       // Bit 49
#define WINNER_MASK        3ULL               // 2 bits
#define WINNER_SHIFT       50
#define MOVES_MASK         0x3FULL        // 6 bits (enough to store ints 0-49)
#define MOVES_SHIFT        52
#define MOVES_INC          (1ULL << MOVES_SHIFT)

/* Current player */
#define PLAYER_X 0
#define PLAYER_O 1

/* Winner states */
#define IN_PROGRESS 0
#define X_WINS    1
#define O_WINS    2

/* Upon making a move */
#define GAME_SUCCESS       0
#define GAME_ERR_NULL     -1
#define GAME_ERR_OVER     -2
#define GAME_ERR_BOUNDS   -3
#define GAME_ERR_OCCUPIED -4

/* Ensures that a coordinate is within bounds (7x7) */
static inline bool is_valid_coord(int r, int c) {
    return (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE);
}

/* Returns PLAYER_X or PLAYER_O (these are macros) */
static inline int get_current_player(const game_t *g) {
    return (g->player_x & TURN_BIT) != 0; // 0 = X, 1 = O
}

/* Returns IN_PROGRESS, X_WINS or O_WINS (these are macros) */
static inline int get_winner(const game_t *g) {
    return (int)((g->player_x >> WINNER_SHIFT) & WINNER_MASK);
}

/* Returns the number of previous moves */
static inline int get_moves_made(const game_t *g) {
    return (int)((g->player_x >> MOVES_SHIFT) & MOVES_MASK);
}

static inline void advance_turn(game_t *g) {
    g->player_x ^= TURN_BIT;
}

static inline void set_winner(game_t *g, int state) {
    g->player_x |= ((uint64_t)state << WINNER_SHIFT);
}

static inline void increment_moves_made(game_t *g) {
    g->player_x += MOVES_INC;
}

bool board_has_line(uint64_t mask, int length);
int game_init(game_t *game);
int make_move(game_t *game, int r, int c);

#endif