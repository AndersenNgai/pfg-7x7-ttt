#include "game.h"

/* Initializes the game, ready for the first move */
int game_init(game_t *game) {
    if (game == NULL)
        return -1;

    // Clears board bits (0-48)
    // Sets current player bit (49) to 0 (PLAYER_X)
    // Game state bits (50-51) to 0 (IN_PROGRESS)
    // Previous moves bits (52-57) to 0
    game->player_x = 0ULL;
    game->player_o = 0ULL;
    return 0;
}


bool board_has_line(uint64_t mask, int length) {
    if (length <= 0 || length > BOARD_SIZE)
        return false;

    // Horizontal
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c <= BOARD_SIZE - length; c++) {
            uint64_t line_mask = 0;
            for (int i = 0; i < length; i++)
                line_mask |= (1ULL << (r * BOARD_SIZE + (c + i)));
            if ((mask & line_mask) == line_mask)
                return true;
        }
    }

    // Vertical
    for (int r = 0; r <= BOARD_SIZE - length; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            uint64_t line_mask = 0;
            for (int i = 0; i < length; i++)
                line_mask |= (1ULL << ((r + i) * BOARD_SIZE + c));
            if ((mask & line_mask) == line_mask)
                return true;
        }
    }

    // Diagonal UL to DR
    for (int r = 0; r <= BOARD_SIZE - length; r++) {
        for (int c = 0; c <= BOARD_SIZE - length; c++) {
            uint64_t line_mask = 0;
            for (int i = 0; i < length; i++)
                line_mask |= (1ULL << ((r + i) * BOARD_SIZE + (c + i)));
            if ((mask & line_mask) == line_mask)
                return true;
        }
    }

    // Diagonal DL to UR
    for (int r = length - 1; r < BOARD_SIZE; r++) {
        for (int c = 0; c <= BOARD_SIZE - length; c++) {
            uint64_t line_mask = 0;
            for (int i = 0; i < length; i++)
                line_mask |= (1ULL << ((r - i) * BOARD_SIZE + (c + i)));
            if ((mask & line_mask) == line_mask)
                return true;
        }
    }

    return false;
}

static void update_game_state(game_t *game, int current_player) {
    increment_moves_made(game);

    uint64_t board = ((uint64_t *)game)[current_player]; // Evil struct manipulation

    if (board_has_line(board, 4)) { // 4 in a row wins
        set_winner(game, current_player + 1); // Evil bit manipulation due to macro definitions for winners and current players
        return;
    }
    if (board_has_line(board, 3)) { // 3 in a row loses
        set_winner(game, 2 - current_player); // Evil bit manipulation. No branch statement needed lol
        return;
    }
    if (get_moves_made(game) >= BOARD_SIZE * BOARD_SIZE)
        set_winner(game, O_WINS);
}

int make_move(game_t *game, int row, int col) {
    if (game == NULL)
        return GAME_ERR_NULL;
    if (get_winner(game) != IN_PROGRESS)
        return GAME_ERR_OVER;
    if (!is_valid_coord(row, col))
        return GAME_ERR_BOUNDS;

    uint64_t square_mask = 1ULL << (row * BOARD_SIZE + col);

    // See if the square is nonempty
    if ((game->player_x | game->player_o) & square_mask)
        return GAME_ERR_OCCUPIED;

    int current_player = get_current_player(game);
    if (current_player == PLAYER_X)
        game->player_x |= square_mask;
    else
        game->player_o |= square_mask;

    update_game_state(game, current_player);

    if (get_winner(game) == IN_PROGRESS)
        advance_turn(game);

    return GAME_SUCCESS;
}