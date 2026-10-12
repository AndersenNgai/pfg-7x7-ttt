#include "board.h"
#include <stdio.h>

int board_init(board_t *board) {
    if (board == NULL)
        return -1;
    board->player_x = 0ULL;
    board->player_o = 0ULL;
    return 0;
}

void print_board(const board_t *board) {
    if (board == NULL)
        return;
    printf("\n  ");
    for (int c = 0; c < BOARD_SIZE; c++) printf(" %d ", c);
    printf("\n +---------------------+\n");
    
    for (int r = 0; r < BOARD_SIZE; r++) {
        printf("%d |", r);
        for (int c = 0; c < BOARD_SIZE; c++) {
            int idx = r * BOARD_SIZE + c;
            uint64_t mask = (1ULL << idx);
            
            char symbol = '.';
            if ((board->player_x & mask) != 0) {
                symbol = 'X';
            } else if ((board->player_o & mask) != 0) {
                symbol = 'O';
            }
            printf(" %c ", symbol);
        }
        printf("|\n");
    }
    printf(" +---------------------+\n");
}