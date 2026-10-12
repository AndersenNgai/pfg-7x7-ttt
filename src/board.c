#include "board.h"
#include <stdio.h>

int board_init(board_t *board) {
    if (board == NULL)
        return -1;
    board->player_x = 0ULL;
    board->player_o = 0ULL;
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