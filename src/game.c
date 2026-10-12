#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int game_init(game_t *game) {
    if (game == NULL)
        return -1;
    board_init(&game->board);
    game->current_player = PLAYER_X;
    game->moves_made = 0;
    game->state = IN_PROGRESS;
    return 0;
}

static inline bool is_valid_coord(int r, int c) {
    return (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE);
}

static void update_game_state(game_t *game) {
    game->moves_made++;

    uint64_t current_mask = (game->current_player == PLAYER_X) ? game->board.player_x : game->board.player_o;
    if (board_has_line(current_mask, 4)) { // 4 in a row
        game->state = (game->current_player == PLAYER_X) ? X_WINS : O_WINS;
        return;
    }
    if (board_has_line(current_mask, 3)) { // 3 in a row
        game->state = (game->current_player == PLAYER_X) ? O_WINS : X_WINS;
        return;
    }
    if (game->moves_made >= BOARD_SIZE * BOARD_SIZE) // No more moves left
        game->state = O_WINS; // X loses by default
}

// Make a move, advance game state
int make_move(game_t *game, int row, int col) {
    if (game == NULL)
        return GAME_ERR_NULL;
    if (game->state != IN_PROGRESS)
        return GAME_ERR_OVER;
    if (!is_valid_coord(row, col)) // Coordinates out of bounds
        return GAME_ERR_BOUNDS;

    uint64_t mask = 1ULL << (row * BOARD_SIZE + col);

    // See if space chosen is occupied
    if ((game->board.player_x & mask) || (game->board.player_o & mask))
        return GAME_ERR_OCCUPIED;

    // Place piece
    if (game->current_player == PLAYER_X)
        game->board.player_x |= mask;
    else
        game->board.player_o |= mask;
    
    update_game_state(game);

    if (game->state == IN_PROGRESS)
        game->current_player = (game->current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;

    return GAME_SUCCESS;
}