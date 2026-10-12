#include <criterion/criterion.h>
#include "game.h"

/* ================================================================= */
/*           Test suites solely for the game_t bitfields             */
/* ================================================================= */

Test(game_bitfield, initialization) {
    game_t game;
    game.player_x = 0xFFFFFFFFFFFFFFFFULL;
    game.player_o = 0xFFFFFFFFFFFFFFFFULL;

    cr_assert_eq(game_init(&game), 0);
    cr_assert_eq(game.player_x, 0ULL);
    cr_assert_eq(game.player_o, 0ULL);
    
    cr_assert_eq(get_current_player(&game), PLAYER_X);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);
    cr_assert_eq(get_moves_made(&game), 0);
}

Test(game_bitfield, null_pointer_fails) {
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

Test(game_bitfield, winner_update) {
    game_t game;
    game_init(&game);

    set_winner(&game, X_WINS);
    
    // X_WINS is 1, shifted by STATE_SHIFT (50) -> (1ULL << 50)
    uint64_t expected_state = ((uint64_t)X_WINS << 50);
    cr_assert_eq(game.player_x, expected_state, "X_WINS state must reside precisely at bits 50-51");

    // Reset and test O_WINS (2)
    game_init(&game);
    set_winner(&game, O_WINS);
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


/* ================================================================= */
/*                  3 in a row, and 4 in a row tests                 */
/* ================================================================= */

Test(line_detection, three_horizontal_boundary_top) {
    uint64_t mask = 0;
    // Row 0, cols 0, 1, 2 (top-left boundary)
    mask |= (1ULL << (0 * BOARD_SIZE + 0));
    mask |= (1ULL << (0 * BOARD_SIZE + 1));
    mask |= (1ULL << (0 * BOARD_SIZE + 2));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row at top-left horizontal boundary");
}

Test(line_detection, three_horizontal_boundary_bottom) {
    uint64_t mask = 0;
    // Row 6, cols 4, 5, 6 (bottom-right boundary)
    mask |= (1ULL << (6 * BOARD_SIZE + 4));
    mask |= (1ULL << (6 * BOARD_SIZE + 5));
    mask |= (1ULL << (6 * BOARD_SIZE + 6));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row at bottom-right horizontal boundary");
}

Test(line_detection, three_horizontal_middle) {
    uint64_t mask = 0;
    // Row 3, cols 2, 3, 4
    mask |= (1ULL << (3 * BOARD_SIZE + 2));
    mask |= (1ULL << (3 * BOARD_SIZE + 3));
    mask |= (1ULL << (3 * BOARD_SIZE + 4));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row in middle horizontal");
}

Test(line_detection, three_horizontal_edge) {
    uint64_t mask = 0;
    // Row 4, cols 0, 1, 2
    mask |= (1ULL << (4 * BOARD_SIZE + 0));
    mask |= (1ULL << (4 * BOARD_SIZE + 1));
    mask |= (1ULL << (4 * BOARD_SIZE + 2));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row on left edge horizontal");
}

Test(line_detection, three_vertical_boundary_left) {
    uint64_t mask = 0;
    // Col 0, rows 0, 1, 2 (top-left vertical boundary)
    mask |= (1ULL << (0 * BOARD_SIZE + 0));
    mask |= (1ULL << (1 * BOARD_SIZE + 0));
    mask |= (1ULL << (2 * BOARD_SIZE + 0));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row at top-left vertical boundary");
}

Test(line_detection, three_vertical_boundary_right) {
    uint64_t mask = 0;
    // Col 6, rows 4, 5, 6 (bottom-right vertical boundary)
    mask |= (1ULL << (4 * BOARD_SIZE + 6));
    mask |= (1ULL << (5 * BOARD_SIZE + 6));
    mask |= (1ULL << (6 * BOARD_SIZE + 6));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row at bottom-right vertical boundary");
}

Test(line_detection, three_vertical_middle) {
    uint64_t mask = 0;
    // Col 3, rows 2, 3, 4
    mask |= (1ULL << (2 * BOARD_SIZE + 3));
    mask |= (1ULL << (3 * BOARD_SIZE + 3));
    mask |= (1ULL << (4 * BOARD_SIZE + 3));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row in middle vertical");
}

Test(line_detection, three_vertical_edge) {
    uint64_t mask = 0;
    // Col 5, rows 0, 1, 2
    mask |= (1ULL << (0 * BOARD_SIZE + 5));
    mask |= (1ULL << (1 * BOARD_SIZE + 5));
    mask |= (1ULL << (2 * BOARD_SIZE + 5));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row on top edge vertical");
}

Test(line_detection, three_ul_dr_corner) {
    uint64_t mask = 0;
    // UL corner: (0,0), (1,1), (2,2)
    mask |= (1ULL << (0 * BOARD_SIZE + 0));
    mask |= (1ULL << (1 * BOARD_SIZE + 1));
    mask |= (1ULL << (2 * BOARD_SIZE + 2));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row starting at UL corner");
}

Test(line_detection, three_dl_ur_corner) {
    uint64_t mask = 0;
    // DL corner going up-right: (6,0), (5,1), (4,2)
    mask |= (1ULL << (6 * BOARD_SIZE + 0));
    mask |= (1ULL << (5 * BOARD_SIZE + 1));
    mask |= (1ULL << (4 * BOARD_SIZE + 2));
    cr_assert(board_has_line(mask, 3), "Should detect 3-in-a-row starting at DL corner");
}

Test(line_detection, gap_does_not_form_three) {
    uint64_t mask = 0;
    mask |= (1ULL << (0 * BOARD_SIZE + 0));
    mask |= (1ULL << (0 * BOARD_SIZE + 1));
    // Skip 2, set 3
    mask |= (1ULL << (0 * BOARD_SIZE + 3));
    cr_assert_not(board_has_line(mask, 3), "Gapped pieces should not register as a 3-in-a-row line");
}

Test(line_detection, exactly_three_not_four) {
    uint64_t mask = 0;
    mask |= (1ULL << (0 * BOARD_SIZE + 1));
    mask |= (1ULL << (0 * BOARD_SIZE + 2));
    mask |= (1ULL << (0 * BOARD_SIZE + 3));
    mask |= (1ULL << (0 * BOARD_SIZE + 5));

    cr_assert(board_has_line(mask, 3), "Should detect the 3-in-a-row");
    cr_assert_not(board_has_line(mask, 4), "Should NOT detect a 4-in-a-row");
}


/* ================================================================= */
/*             (Continued) 3 in a row, and 4 in a row tests          */
/* ================================================================= */

Test(line_detection, four_horizontal_top_boundary) {
    uint64_t mask = 0;
    for (int c = 0; c < 4; c++) mask |= (1ULL << (0 * BOARD_SIZE + c));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row at top horizontal boundary");
}

Test(line_detection, four_horizontal_bottom_boundary) {
    uint64_t mask = 0;
    for (int c = 3; c < 7; c++) mask |= (1ULL << (6 * BOARD_SIZE + c));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row at bottom horizontal boundary");
}

Test(line_detection, four_horizontal_middle) {
    uint64_t mask = 0;
    for (int c = 1; c < 5; c++) mask |= (1ULL << (3 * BOARD_SIZE + c));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row in middle horizontal");
}

Test(line_detection, four_horizontal_right_edge) {
    uint64_t mask = 0;
    for (int c = 3; c < 7; c++) mask |= (1ULL << (2 * BOARD_SIZE + c));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row on right edge horizontal");
}

Test(line_detection, four_vertical_left_boundary) {
    uint64_t mask = 0;
    for (int r = 0; r < 4; r++) mask |= (1ULL << (r * BOARD_SIZE + 0));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row at left vertical boundary");
}

Test(line_detection, four_vertical_right_boundary) {
    uint64_t mask = 0;
    for (int r = 3; r < 7; r++) mask |= (1ULL << (r * BOARD_SIZE + 6));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row at right vertical boundary");
}

Test(line_detection, four_vertical_middle) {
    uint64_t mask = 0;
    for (int r = 1; r < 5; r++) mask |= (1ULL << (r * BOARD_SIZE + 3));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row in middle vertical");
}

Test(line_detection, four_ul_dr_boundary) {
    uint64_t mask = 0;
    for (int i = 0; i < 4; i++) mask |= (1ULL << ((0 + i) * BOARD_SIZE + (0 + i)));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row along UL-DR corner");
}

Test(line_detection, four_dl_ur_boundary) {
    uint64_t mask = 0;
    for (int i = 0; i < 4; i++) mask |= (1ULL << ((6 - i) * BOARD_SIZE + (0 + i)));
    cr_assert(board_has_line(mask, 4), "Should detect 4-in-a-row along DL-UR corner");
}

Test(line_detection, gap_does_not_form_four) {
    uint64_t mask = 0;
    mask |= (1ULL << (0 * BOARD_SIZE + 0));
    mask |= (1ULL << (0 * BOARD_SIZE + 1));
    mask |= (1ULL << (0 * BOARD_SIZE + 2));
    mask |= (1ULL << (0 * BOARD_SIZE + 4));
    cr_assert_not(board_has_line(mask, 4), "Gapped pieces should not register as a 4-in-a-row line");
}

Test(line_detection, exactly_four_not_five) {
    uint64_t mask = 0;
    mask |= (1ULL << (0 * BOARD_SIZE + 1));
    mask |= (1ULL << (0 * BOARD_SIZE + 2));
    mask |= (1ULL << (0 * BOARD_SIZE + 3));
    mask |= (1ULL << (0 * BOARD_SIZE + 4));
    mask |= (1ULL << (0 * BOARD_SIZE + 6));

    cr_assert(board_has_line(mask, 4), "Should detect the 4-in-a-row");
    cr_assert_not(board_has_line(mask, 5), "Should NOT detect a 5-in-a-row");
}


/* ================================================================= */
/*                  Additional line detection tests                  */
/* ================================================================= */

Test(line_detection, horizontal_does_not_wrap_across_rows) {
    uint64_t mask = 0;
    // Place 4 pieces spanning across the end of row 0 and the start of row 1
    mask |= (1ULL << (0 * BOARD_SIZE + 5));
    mask |= (1ULL << (0 * BOARD_SIZE + 6));
    mask |= (1ULL << (1 * BOARD_SIZE + 0));
    mask |= (1ULL << (1 * BOARD_SIZE + 1));

    cr_assert_not(board_has_line(mask, 3), "Horizontal lines must not wrap around from one row to the next");
}

Test(line_detection, empty_board_has_no_lines) {
    uint64_t mask = 0ULL;
    cr_assert_not(board_has_line(mask, 3), "An empty board mask must not contain a 3-in-a-row");
    cr_assert_not(board_has_line(mask, 1), "An empty board mask must not contain a 1-in-a-row");
}

Test(line_detection, invalid_line_lengths_return_false) {
    uint64_t mask = ~0ULL; // Fully populated mask to test bounds logic
    cr_assert_not(board_has_line(mask, 0), "Length 0 should be invalid and return false");
    cr_assert_not(board_has_line(mask, -1), "Negative length should be invalid and return false");
    cr_assert_not(board_has_line(mask, BOARD_SIZE + 1), "Length greater than BOARD_SIZE (7) should return false");
}


/* ================================================================= */
/*             make_move() correctness and error cases               */
/* ================================================================= */

Test(make_move, basic_state_checks) {
    game_t game;
    game_init(&game);

    // Player X makes move at (0, 0)
    int res1 = make_move(&game, 0, 0);
    cr_assert_eq(res1, GAME_SUCCESS);
    cr_assert_eq(get_moves_made(&game), 1);
    cr_assert_eq(get_current_player(&game), PLAYER_O);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);
    cr_assert_eq(game.player_x & (1ULL << 0), (1ULL << 0));
    cr_assert_eq(game.player_o, 0ULL);

    // Player O makes move at (1, 1)
    int res2 = make_move(&game, 1, 1);
    cr_assert_eq(res2, GAME_SUCCESS);
    cr_assert_eq(get_moves_made(&game), 2);
    cr_assert_eq(get_current_player(&game), PLAYER_X);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);
    cr_assert_eq(game.player_x & (1ULL << 0), (1ULL << 0));
    cr_assert_eq(game.player_o & (1ULL << (1 * BOARD_SIZE + 1)), (1ULL << (1 * BOARD_SIZE + 1)));
}

Test(make_move, null_pointer_fails) {
    int res = make_move(NULL, 0, 0);
    cr_assert_eq(res, GAME_ERR_NULL);
}

Test(make_move, invalid_square_fails) {
    game_t game;
    game_init(&game);

    // Too high
    cr_assert_eq(make_move(&game, -1, 3), GAME_ERR_BOUNDS);
    cr_assert_eq(get_winner(&game), IN_PROGRESS, "State should remain IN_PROGRESS after out-of-bounds move");
    cr_assert_eq(get_moves_made(&game), 0, "Move count should not increment on out-of-bounds error");
    cr_assert_eq(get_current_player(&game), PLAYER_X, "Turn should not advance on failed move");

    // Too far left
    cr_assert_eq(make_move(&game, 3, -1), GAME_ERR_BOUNDS);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);
    cr_assert_eq(get_moves_made(&game), 0);
    cr_assert_eq(get_current_player(&game), PLAYER_X);

    // Too low
    cr_assert_eq(make_move(&game, 7, 3), GAME_ERR_BOUNDS);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);
    cr_assert_eq(get_moves_made(&game), 0);
    cr_assert_eq(get_current_player(&game), PLAYER_X);

    // Too far right
    cr_assert_eq(make_move(&game, 3, 7), GAME_ERR_BOUNDS);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);
    cr_assert_eq(get_moves_made(&game), 0);
    cr_assert_eq(get_current_player(&game), PLAYER_X);

    cr_assert_eq(game.player_x, 0ULL);
    cr_assert_eq(game.player_o, 0ULL);
}

Test(make_move, game_over_prevents_moves) {
    game_t game;
    game_init(&game);
    // Force game winner state to X_WINS
    set_winner(&game, X_WINS);

    cr_assert_eq(make_move(&game, 2, 2), GAME_ERR_OVER);
}

Test(make_move, occupied_square_same_player_fails) {
    game_t game;
    game_init(&game);

    // X plays valid move at (2, 2)
    cr_assert_eq(make_move(&game, 2, 2), GAME_SUCCESS);
    cr_assert_eq(get_current_player(&game), PLAYER_O);
    cr_assert_eq(get_moves_made(&game), 1);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);

    // Force turn back to X to test trying to overwrite their own piece
    advance_turn(&game); // Now back to PLAYER_X

    // Attempt to play on the same square (2, 2)
    cr_assert_eq(make_move(&game, 2, 2), GAME_ERR_OCCUPIED);

    // Verify state did not change after the failed move
    cr_assert_eq(get_current_player(&game), PLAYER_X, "Turn should not change on occupancy error");
    cr_assert_eq(get_moves_made(&game), 1, "Move count must remain 1");
    cr_assert_eq(get_winner(&game), IN_PROGRESS, "Winner status must remain IN_PROGRESS");

    uint64_t expected_x_mask = (1ULL << (2 * BOARD_SIZE + 2));
    cr_assert_eq(game.player_x & BOARD_MASK, expected_x_mask, "Player X board should only contain the original piece at (2, 2)");
    cr_assert_eq(game.player_o, 0ULL, "Player O board must remain empty");
}

Test(make_move, occupied_square_opponent_player_fails) {
    game_t game;
    game_init(&game);

    // Manually place an O piece at (4, 4)
    uint64_t o_piece_mask = (1ULL << (4 * BOARD_SIZE + 4));
    game.player_o |= o_piece_mask;

    // Current player is X. Attempt to play at (4, 4) where O already is.
    cr_assert_eq(get_current_player(&game), PLAYER_X);
    cr_assert_eq(get_moves_made(&game), 0);

    cr_assert_eq(make_move(&game, 4, 4), GAME_ERR_OCCUPIED);

    // Verify state is completely preserved
    cr_assert_eq(get_current_player(&game), PLAYER_X, "Turn should remain X");
    cr_assert_eq(get_moves_made(&game), 0, "Move count must remain 0");
    cr_assert_eq(get_winner(&game), IN_PROGRESS, "Winner status must remain IN_PROGRESS");
    cr_assert_eq(game.player_o & (1ULL << (4 * BOARD_SIZE + 4)), (1ULL << (4 * BOARD_SIZE + 4)), "Opponent piece should remain untouched");
    cr_assert_eq(game.player_o & BOARD_MASK, o_piece_mask, "Opponent piece at (4, 4) should remain untouched");
    cr_assert_eq(game.player_x & BOARD_MASK, 0ULL, "Player X board must remain empty after failed move");
}


/* ================================================================= */
/*                        Integration tests                          */
/* ================================================================= */

Test(integration, ten_moves_no_winner) {
    game_t game;
    game_init(&game);

    // Play 10 alternating moves safely apart to avoid 3 or 4 in a row
    int coords[10][2] = {
        {0, 0}, {6, 6},
        {0, 2}, {6, 4},
        {2, 0}, {4, 6},
        {2, 2}, {4, 4},
        {4, 0}, {2, 6}
    };

    for (int i = 0; i < 10; i++) {
        cr_assert_eq(make_move(&game, coords[i][0], coords[i][1]), GAME_SUCCESS);
    }

    cr_assert_eq(get_moves_made(&game), 10);
    cr_assert_eq(get_winner(&game), IN_PROGRESS);

    // Verify exact expected masks for both players after 10 moves
    uint64_t expected_x = (1ULL << (0 * BOARD_SIZE + 0)) |
                          (1ULL << (0 * BOARD_SIZE + 2)) |
                          (1ULL << (2 * BOARD_SIZE + 0)) |
                          (1ULL << (2 * BOARD_SIZE + 2)) |
                          (1ULL << (4 * BOARD_SIZE + 0));
                          
    uint64_t expected_o = (1ULL << (6 * BOARD_SIZE + 6)) |
                          (1ULL << (6 * BOARD_SIZE + 4)) |
                          (1ULL << (4 * BOARD_SIZE + 6)) |
                          (1ULL << (4 * BOARD_SIZE + 4)) |
                          (1ULL << (2 * BOARD_SIZE + 6));

    cr_assert_eq(game.player_x & BOARD_MASK, expected_x, "Player X board mask does not match expected positions");
    cr_assert_eq(game.player_o & BOARD_MASK, expected_o, "Player O board mask does not match expected positions");
}

Test(integration, player_x_wins_on_fourth_move) {
    game_t game;
    game_init(&game);

    // X plays horizontal line of 4: (0,0), (0,1), (0,3), (0,2)
    // O plays non-threatening squares: (6,0), (6,1), (6,3)
    cr_assert_eq(make_move(&game, 0, 0), GAME_SUCCESS); // X move 1
    cr_assert_eq(make_move(&game, 6, 0), GAME_SUCCESS); // O move 1
    cr_assert_eq(make_move(&game, 0, 1), GAME_SUCCESS); // X move 2
    cr_assert_eq(make_move(&game, 6, 1), GAME_SUCCESS); // O move 2
    cr_assert_eq(make_move(&game, 0, 3), GAME_SUCCESS); // X move 3
    cr_assert_eq(make_move(&game, 6, 3), GAME_SUCCESS); // O move 3
    cr_assert_eq(make_move(&game, 0, 2), GAME_SUCCESS); // X move 4 -> 4 in a row!

    cr_assert_eq(get_winner(&game), X_WINS);
    cr_assert_eq(get_moves_made(&game), 7);

    // Verify final board accuracy for X's winning line and O's pieces
    uint64_t expected_x = (1ULL << (0 * BOARD_SIZE + 0)) |
                          (1ULL << (0 * BOARD_SIZE + 1)) |
                          (1ULL << (0 * BOARD_SIZE + 3)) |
                          (1ULL << (0 * BOARD_SIZE + 2));

    uint64_t expected_o = (1ULL << (6 * BOARD_SIZE + 0)) |
                          (1ULL << (6 * BOARD_SIZE + 1)) |
                          (1ULL << (6 * BOARD_SIZE + 3));

    cr_assert_eq(game.player_x & BOARD_MASK, expected_x, "Player X winning board mask is incorrect");
    cr_assert_eq(game.player_o & BOARD_MASK, expected_o, "Player O board mask is incorrect");
}

Test(integration, player_o_wins_on_fourth_move) {
    game_t game;
    game_init(&game);

    // X plays safe: (5,0), (5,1), (5,3)
    // O plays vertical line of 4 starting on their turn: (1,1), (2,1), (4,1), (3,1)
    cr_assert_eq(make_move(&game, 5, 0), GAME_SUCCESS); // X 1
    cr_assert_eq(make_move(&game, 1, 1), GAME_SUCCESS); // O 1
    cr_assert_eq(make_move(&game, 5, 1), GAME_SUCCESS); // X 2
    cr_assert_eq(make_move(&game, 2, 1), GAME_SUCCESS); // O 2
    cr_assert_eq(make_move(&game, 5, 3), GAME_SUCCESS); // X 3
    cr_assert_eq(make_move(&game, 4, 1), GAME_SUCCESS); // O 3
    // X makes a non-winning move
    cr_assert_eq(make_move(&game, 5, 4), GAME_SUCCESS); // X 4
    cr_assert_eq(make_move(&game, 3, 1), GAME_SUCCESS); // O 4 -> O gets 4 in a row!

    cr_assert_eq(get_winner(&game), O_WINS);

    // Verify final board accuracy for O's winning line and X's pieces
    uint64_t expected_x = (1ULL << (5 * BOARD_SIZE + 0)) |
                          (1ULL << (5 * BOARD_SIZE + 1)) |
                          (1ULL << (5 * BOARD_SIZE + 3)) |
                          (1ULL << (5 * BOARD_SIZE + 4));

    uint64_t expected_o = (1ULL << (1 * BOARD_SIZE + 1)) |
                          (1ULL << (2 * BOARD_SIZE + 1)) |
                          (1ULL << (4 * BOARD_SIZE + 1)) |
                          (1ULL << (3 * BOARD_SIZE + 1));

    cr_assert_eq(game.player_x & BOARD_MASK, expected_x, "Player X board mask is incorrect");
    cr_assert_eq(game.player_o & BOARD_MASK, expected_o, "Player O winning board mask is incorrect");
}

Test(integration, board_full_49th_move_loss_default) {
    game_t game;
    game_init(&game);

    // Manually set moves made to 48
    game.player_x |= (48ULL << MOVES_SHIFT);

    // Find an empty square (e.g., (3, 3))
    cr_assert_eq(get_winner(&game), IN_PROGRESS);
    cr_assert_eq(make_move(&game, 3, 3), GAME_SUCCESS);

    // Since moves reached 49 and no 4-in-a-row occurred, default rule triggers O win
    cr_assert_eq(get_winner(&game), O_WINS);
    cr_assert_eq(get_moves_made(&game), 49);

    // Verify the final move was correctly recorded onto X's board mask
    uint64_t expected_x = (1ULL << (3 * BOARD_SIZE + 3));
    cr_assert_eq(game.player_x & BOARD_MASK, expected_x, "Player X board mask should contain the 49th move at (3, 3)");
    cr_assert_eq(game.player_o & BOARD_MASK, 0ULL, "Player O board mask should remain empty");
}

Test(integration, full_game_seven_moves_x_wins_then_o_rejected) {
    game_t game;
    game_init(&game);

    cr_assert_eq(make_move(&game, 0, 0), GAME_SUCCESS); // X 1
    cr_assert_eq(make_move(&game, 6, 0), GAME_SUCCESS); // O 1
    cr_assert_eq(make_move(&game, 0, 1), GAME_SUCCESS); // X 2
    cr_assert_eq(make_move(&game, 6, 1), GAME_SUCCESS); // O 2
    cr_assert_eq(make_move(&game, 0, 3), GAME_SUCCESS); // X 3
    cr_assert_eq(make_move(&game, 6, 3), GAME_SUCCESS); // O 3
    cr_assert_eq(make_move(&game, 0, 2), GAME_SUCCESS); // X 4 -> X gets 4-in-a-row!

    // Verify X won on move 7
    cr_assert_eq(get_winner(&game), X_WINS);
    cr_assert_eq(get_moves_made(&game), 7);

    // O attempts to make the 8th move, but the game is already over
    cr_assert_eq(make_move(&game, 4, 4), GAME_ERR_OVER, "Game over status should reject subsequent moves");

    // Verify that O's attempted move did not alter the board state
    uint64_t expected_x = (1ULL << (0 * BOARD_SIZE + 0)) |
                          (1ULL << (0 * BOARD_SIZE + 1)) |
                          (1ULL << (0 * BOARD_SIZE + 3)) |
                          (1ULL << (0 * BOARD_SIZE + 2));

    uint64_t expected_o = (1ULL << (6 * BOARD_SIZE + 0)) |
                          (1ULL << (6 * BOARD_SIZE + 1)) |
                          (1ULL << (6 * BOARD_SIZE + 3));

    cr_assert_eq(game.player_x & BOARD_MASK, expected_x, "Player X board mask should remain unchanged after rejection");
    cr_assert_eq(game.player_o & BOARD_MASK, expected_o, "Player O board mask should remain unchanged after rejection");
}