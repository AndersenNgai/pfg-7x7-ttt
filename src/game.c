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