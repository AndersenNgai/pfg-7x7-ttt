#include "game.h"
#include <stdbool.h>
#include <stddef.h>

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

// Helper to check win/loss/draw conditions based on 7x7 variant rules
static void update_game_state(game_t *game) {
    // TODO: Integrate full 3-in-a-row (loss) / 4-in-a-row (win) evaluation logic here.
    // For now, check if the board is full (49 moves on a 7x7 board)
    if (game->moves_made >= BOARD_SIZE * BOARD_SIZE)
        game->state = O_WINS; // X loses by default
    game->moves_made++;
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