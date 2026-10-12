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