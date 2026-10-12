#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define BOARD_SIZE 7

typedef struct {
    uint64_t player_x;
    uint64_t player_o;
} board_t;

int board_init(board_t *board);
void print_board(const board_t *board);

#endif