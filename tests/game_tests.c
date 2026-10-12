#include <criterion/criterion.h>
#include "game.h"

/* Test suites solely for the game_t bitfields */

Test(game_bitfield, initialization) {
    game_t game;
    game.player_x = 0xFFFFFFFFFFFFFFFFULL;
    game.player_o = 0xFFFFFFFFFFFFFFFFULL;

    cr_assert_eq(game_init(&game), 0);
    cr_assert_eq(game.player_x, 0ULL);
    cr_assert_eq(game.player_o, 0ULL);
    
    cr_assert_eq(get_current_player(&game), PLAYER_X);
    cr_assert_eq(get_game_state(&game), IN_PROGRESS);
    cr_assert_eq(get_moves_made(&game), 0);
}

Test(game_bitfield, null_initialization_fails) {
    int result = game_init(NULL);
    cr_assert_eq(result, -1, "game_init should return -1 when passed NULL");
}

Test(game_bitfield, turn_advancing) {
    game_t game;
    game_init(&game);

    advance_turn(&game);
    
    // Check that bit 49 is set to 1, and nothing else changed
    uint64_t expected_player_x = (1ULL << 49);
    cr_assert_eq(game.player_x, expected_player_x, "Player X register should only have bit 49 set after advancing turn");

    advance_turn(&game);
    cr_assert_eq(game.player_x, 0ULL, "Player X register should return to 0 after second turn toggle");
}

Test(game_bitfield, game_state_updates) {
    game_t game;
    game_init(&game);

    set_game_state(&game, X_WINS);
    
    // X_WINS is 1, shifted by STATE_SHIFT (50) -> (1ULL << 50)
    uint64_t expected_state = ((uint64_t)X_WINS << 50);
    cr_assert_eq(game.player_x, expected_state, "X_WINS state must reside precisely at bits 50-51");

    // Reset and test O_WINS (2)
    game_init(&game);
    set_game_state(&game, O_WINS);
    uint64_t expected_state_o = ((uint64_t)O_WINS << 50);
    cr_assert_eq(game.player_x, expected_state_o, "O_WINS state must reside precisely at bits 50-51");
}

Test(game_bitfield, moves_incrementing) {
    game_t game;
    game_init(&game);

    increment_moves_made(&game);
    
    // Since moves start at bit 52, incrementing once should make the register equal (1ULL << 52)
    uint64_t expected_moves_1 = (1ULL << 52);
    cr_assert_eq(game.player_x, expected_moves_1, "Moves made = 1 should occupy bit 52");

    increment_moves_made(&game);
    // Value 2 at bit 52 means (2ULL << 52)
    uint64_t expected_moves_2 = (2ULL << 52);
    cr_assert_eq(game.player_x, expected_moves_2, "Moves made = 2 should represent binary 10 at bits 52-57");

    // Do 5 more incrementations (bringing total to 7)
    for (int i = 0; i < 5; i++) {
        increment_moves_made(&game);
    }
    uint64_t expected_moves_7 = (7ULL << 52);
    cr_assert_eq(game.player_x, expected_moves_7, "Moves made = 7 should represent binary 111 at bits 52-57");

    // Do 1 more incrementation (bringing total to 8)
    increment_moves_made(&game);
    uint64_t expected_moves_8 = (8ULL << 52);
    cr_assert_eq(game.player_x, expected_moves_8, "Moves made = 8 should carry cleanly into the 4th bit position (8ULL << 52)");
}