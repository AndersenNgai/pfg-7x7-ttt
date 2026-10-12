#include <criterion/criterion.h>
#include "game.h"

// Helper to convert player enum to string for clean failure messages
static const char* player_to_str(player_t p) {
    switch (p) {
        case PLAYER_X: return "PLAYER_X";
        case PLAYER_O: return "PLAYER_O";
        default:       return "UNKNOWN";
    }
}

// Helper to convert game state enum to string for clean failure messages
static const char* state_to_str(game_state_t s) {
    switch (s) {
        case IN_PROGRESS: return "IN_PROGRESS";
        case X_WINS:      return "X_WINS";
        case O_WINS:      return "O_WINS";
        default:          return "UNKNOWN";
    }
}

// Helper to assert proper game state with detailed error feedback
static void assert_game_state(const game_t *game, player_t expected_player, game_state_t expected_state, int expected_moves) {
    cr_assert_eq(
        game->current_player, expected_player,
        "Current player mismatch: expected %s, but got %s",
        player_to_str(expected_player), player_to_str(game->current_player)
    );
    cr_assert_eq(
        game->state, expected_state,
        "Game state mismatch: expected %s, but got %s",
        state_to_str(expected_state), state_to_str(game->state)
    );
    cr_assert_eq(
        game->moves_made, expected_moves,
        "Moves made count mismatch: expected %d, but got %d",
        expected_moves, game->moves_made
    );
}

// Enum for square occupancy
typedef enum {
    SQUARE_EMPTY = 0,
    SQUARE_X,
    SQUARE_O
} square_state_t;

// Helper identify a square's occupancy
static void assert_square(const game_t *game, int r, int c, square_state_t expected) {
    uint64_t mask = 1ULL << (r * BOARD_SIZE + c);
    bool has_x = (game->board.player_x & mask) != 0;
    bool has_o = (game->board.player_o & mask) != 0;

    // Invariant: A square can never be occupied by both players at the same time
    cr_assert(
        !(has_x && has_o),
        "Board corruption invariant violation: Square (%d, %d) is claimed by both PLAYER_X and PLAYER_O",
        r, c
    );

    square_state_t actual = SQUARE_EMPTY;
    if (has_x) actual = SQUARE_X;
    else if (has_o) actual = SQUARE_O;

    const char *expected_str = (expected == SQUARE_X) ? "PLAYER_X" : (expected == SQUARE_O) ? "PLAYER_O" : "EMPTY";
    const char *actual_str = (actual == SQUARE_X) ? "PLAYER_X" : (actual == SQUARE_O) ? "PLAYER_O" : "EMPTY";

    cr_assert_eq(
        actual, expected,
        "Square (%d, %d) ownership mismatch: expected %s, but got %s",
        r, c, expected_str, actual_str
    );
}

Test(game_suite, test_game_init_success) {
    game_t game;
    int result = game_init(&game);
    
    cr_assert_eq(result, 0, "game_init should return 0 on success");
    cr_assert_eq(game.board.player_o, 0ULL, "Game board player_o should be 0");
    cr_assert_eq(game.board.player_x, 0ULL, "Game board player_x should be 0");
    assert_game_state(&game, PLAYER_X, IN_PROGRESS, 0);
}

Test(game_suite, test_game_init_null) {
    int result = game_init(NULL);
    cr_assert_eq(result, -1, "game_init should return -1 when passed NULL");
}

Test(game_suite, test_make_move_success) {
    game_t game;
    game_init(&game);

    // Player X plays at (4, 3)
    int status = make_move(&game, 4, 3);
    cr_assert_eq(status, GAME_SUCCESS, "Valid move for X should return GAME_SUCCESS");
    assert_game_state(&game, PLAYER_O, IN_PROGRESS, 1);
    assert_square(&game, 4, 3, SQUARE_X);

    // Player O plays at (3, 3)
    status = make_move(&game, 3, 3);
    cr_assert_eq(status, GAME_SUCCESS, "Valid move for O should return GAME_SUCCESS");
    assert_game_state(&game, PLAYER_X, IN_PROGRESS, 2);
    assert_square(&game, 3, 3, SQUARE_O);
}

Test(game_suite, test_make_move_null_pointer) {
    int status = make_move(NULL, 0, 0);
    cr_assert_eq(status, GAME_ERR_NULL, "Null game pointer should return GAME_ERR_NULL");
}

Test(game_suite, test_make_move_out_of_bounds) {
    game_t game;
    game_init(&game);

    cr_assert_eq(make_move(&game, -1, 3), GAME_ERR_BOUNDS, "Negative row should fail with GAME_ERR_BOUNDS");
    assert_game_state(&game, PLAYER_X, IN_PROGRESS, 0);

    cr_assert_eq(make_move(&game, 7, 3), GAME_ERR_BOUNDS, "Row 7 (out of bounds) should fail");
    assert_game_state(&game, PLAYER_X, IN_PROGRESS, 0);

    cr_assert_eq(make_move(&game, 3, -1), GAME_ERR_BOUNDS, "Negative column should fail");
    assert_game_state(&game, PLAYER_X, IN_PROGRESS, 0);

    cr_assert_eq(make_move(&game, 3, 7), GAME_ERR_BOUNDS, "Column 7 (out of bounds) should fail");
    assert_game_state(&game, PLAYER_X, IN_PROGRESS, 0);
}

Test(game_suite, test_make_move_occupied_cell) {
    game_t game;
    game_init(&game);

    // Player X plays at (2, 2)
    cr_assert_eq(make_move(&game, 2, 2), GAME_SUCCESS, "First move at (2,2) should succeed");
    assert_game_state(&game, PLAYER_O, IN_PROGRESS, 1);
    assert_square(&game, 2, 2, SQUARE_X);
    
    // Player O tries to play at the same spot (2, 2)
    cr_assert_eq(make_move(&game, 2, 2), GAME_ERR_OCCUPIED, "Playing on an occupied cell should return GAME_ERR_OCCUPIED");
    assert_game_state(&game, PLAYER_O, IN_PROGRESS, 1);
    assert_square(&game, 2, 2, SQUARE_X);
}