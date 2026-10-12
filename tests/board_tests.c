#include <criterion/criterion.h>
#include "../include/board.h"

Test(board_suite, test_board_init_success) {
    board_t board;
    int result = board_init(&board);
    
    cr_assert_eq(result, 0, "board_init should return 0 on success");
    cr_assert_eq(board.player_x, 0ULL, "player_x bitboard should be initialized to 0");
    cr_assert_eq(board.player_o, 0ULL, "player_o bitboard should be initialized to 0");
}

Test(board_suite, test_board_init_null) {
    int result = board_init(NULL);
    cr_assert_eq(result, -1, "board_init should return -1 when passed NULL");
}

Test(board_suite, test_board_has_line_invalid_lengths) {
    uint64_t mask = ~0ULL; // Fully populated mask

    cr_assert(!board_has_line(mask, 0), "Length 0 should return false");
    cr_assert(!board_has_line(mask, -1), "Negative length should return false");
    cr_assert(!board_has_line(mask, BOARD_SIZE + 1), "Length greater than BOARD_SIZE should return false");
}

// --- HORIZONTAL BOUNDARY TESTS (4 tests) ---

Test(board_suite, test_board_has_line_horizontal_left_edge) {
    uint64_t mask = 0ULL;
    // Row 0, columns 0, 1, 2, 3 (Left boundary)
    for (int c = 0; c < 4; c++) {
        mask |= (1ULL << (0 * BOARD_SIZE + c));
    }
    cr_assert(board_has_line(mask, 4), "Should detect horizontal length 4 on the left edge of row 0");
}

Test(board_suite, test_board_has_line_horizontal_left_edge_offset) {
    uint64_t mask = 0ULL;
    // Row 3, columns 0, 1, 2, 3 (Left boundary, middle row)
    for (int c = 0; c < 4; c++) {
        mask |= (1ULL << (3 * BOARD_SIZE + c));
    }
    cr_assert(board_has_line(mask, 4), "Should detect horizontal length 4 on the left edge of row 3");
}

Test(board_suite, test_board_has_line_horizontal_right_edge) {
    uint64_t mask = 0ULL;
    // Row 0, columns 3, 4, 5, 6 (Right boundary)
    for (int c = 3; c < BOARD_SIZE; c++) {
        mask |= (1ULL << (0 * BOARD_SIZE + c));
    }
    cr_assert(board_has_line(mask, 4), "Should detect horizontal length 4 on the right edge of row 0");
}

Test(board_suite, test_board_has_line_horizontal_right_edge_bottom) {
    uint64_t mask = 0ULL;
    // Row 6, columns 3, 4, 5, 6 (Right boundary, bottom row)
    for (int c = 3; c < BOARD_SIZE; c++) {
        mask |= (1ULL << (6 * BOARD_SIZE + c));
    }
    cr_assert(board_has_line(mask, 4), "Should detect horizontal length 4 on the right edge of row 6");
}

// --- VERTICAL BOUNDARY TESTS (4 tests) ---

Test(board_suite, test_board_has_line_vertical_top_edge) {
    uint64_t mask = 0ULL;
    // Column 0, rows 0, 1, 2, 3 (Top boundary)
    for (int r = 0; r < 4; r++) {
        mask |= (1ULL << (r * BOARD_SIZE + 0));
    }
    cr_assert(board_has_line(mask, 4), "Should detect vertical length 4 on the top edge of column 0");
}

Test(board_suite, test_board_has_line_vertical_top_edge_offset) {
    uint64_t mask = 0ULL;
    // Column 3, rows 0, 1, 2, 3 (Top boundary, middle column)
    for (int r = 0; r < 4; r++) {
        mask |= (1ULL << (r * BOARD_SIZE + 3));
    }
    cr_assert(board_has_line(mask, 4), "Should detect vertical length 4 on the top edge of column 3");
}

Test(board_suite, test_board_has_line_vertical_bottom_edge) {
    uint64_t mask = 0ULL;
    // Column 0, rows 3, 4, 5, 6 (Bottom boundary)
    for (int r = 3; r < BOARD_SIZE; r++) {
        mask |= (1ULL << (r * BOARD_SIZE + 0));
    }
    cr_assert(board_has_line(mask, 4), "Should detect vertical length 4 on the bottom edge of column 0");
}

Test(board_suite, test_board_has_line_vertical_bottom_edge_right) {
    uint64_t mask = 0ULL;
    // Column 6, rows 3, 4, 5, 6 (Bottom boundary, right-most column)
    for (int r = 3; r < BOARD_SIZE; r++) {
        mask |= (1ULL << (r * BOARD_SIZE + 6));
    }
    cr_assert(board_has_line(mask, 4), "Should detect vertical length 4 on the bottom edge of column 6");
}

// --- DIAGONAL UL -> DR BOUNDARY TESTS (2 tests) ---

Test(board_suite, test_board_has_line_diagonal_ul_to_dr_top_left) {
    uint64_t mask = 0ULL;
    // Starting at top-left corner (0,0) down-right
    for (int i = 0; i < 4; i++) {
        mask |= (1ULL << ((0 + i) * BOARD_SIZE + (0 + i)));
    }
    cr_assert(board_has_line(mask, 4), "Should detect UL->DR diagonal starting at top-left corner (0,0)");
}

Test(board_suite, test_board_has_line_diagonal_ul_to_dr_bottom_right) {
    uint64_t mask = 0ULL;
    // Starting at offset (3,3) down to bottom-right corner (6,6)
    for (int i = 0; i < 4; i++) {
        mask |= (1ULL << ((3 + i) * BOARD_SIZE + (3 + i)));
    }
    cr_assert(board_has_line(mask, 4), "Should detect UL->DR diagonal terminating at bottom-right corner (6,6)");
}

// --- DIAGONAL DL -> UR BOUNDARY TESTS (2 tests) ---

Test(board_suite, test_board_has_line_diagonal_dl_to_ur_bottom_left) {
    uint64_t mask = 0ULL;
    // Starting at bottom-left corner (6,0) up-right
    for (int i = 0; i < 4; i++) {
        mask |= (1ULL << ((6 - i) * BOARD_SIZE + (0 + i)));
    }
    cr_assert(board_has_line(mask, 4), "Should detect DL->UR diagonal starting at bottom-left corner (6,0)");
}

Test(board_suite, test_board_has_line_diagonal_dl_to_ur_top_right) {
    uint64_t mask = 0ULL;
    // Starting from row 3, col 3 up to top-right corner (0,6)
    for (int i = 0; i < 4; i++) {
        mask |= (1ULL << ((3 - i) * BOARD_SIZE + (3 + i)));
    }
    cr_assert(board_has_line(mask, 4), "Should detect DL->UR diagonal terminating at top-right corner (0,6)");
}

Test(board_suite, test_board_has_line_no_false_positives) {
    uint64_t mask = 0ULL;
    mask |= (1ULL << (0 * BOARD_SIZE + 0));
    mask |= (1ULL << (2 * BOARD_SIZE + 3));
    mask |= (1ULL << (4 * BOARD_SIZE + 1));
    mask |= (1ULL << (6 * BOARD_SIZE + 6));

    cr_assert(!board_has_line(mask, 3), "Scattered bits should not trigger length 3");
    cr_assert(!board_has_line(mask, 4), "Scattered bits should not trigger length 4");
}