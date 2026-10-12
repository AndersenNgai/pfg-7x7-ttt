#include <stdio.h>
#include "game.h"

int play() {
    game_t game;
    if (game_init(&game) != 0) {
        perror("Failed to initialize game");
        return -1;
    }

    printf("========================================\n");
    printf("     PFG 7x7 TTT Interactive Engine     \n");
    printf("========================================\n");
    printf("Rules:\n");
    printf(" - Get 4-in-a-row to WIN.\n");
    printf(" - Getting 3-in-a-row causes immediate LOSS.\n");
    printf(" - Board fills up (49 moves) -> Default loss for X.\n\n");

    while (get_winner(&game) == IN_PROGRESS) {
        print_board(&game);

        int current = get_current_player(&game);
        printf("Player %s's turn.\n", current == PLAYER_X ? "X" : "O");
        printf("Enter row and column (e.g., '3 3'): ");

        int r, c;
        if (scanf("%d %d", &r, &c) != 2) {
            printf("\n[Error] Invalid input format. Please enter two integers separated by a space.\n");
            // Clear input buffer to prevent infinite loops on bad input
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        int result = make_move(&game, r, c);
        if (result != GAME_SUCCESS) {
            printf("\n[Move Rejected]: ");
            switch (result) {
                case GAME_ERR_BOUNDS:
                    printf("Coordinates must be between 0 and 6.\n");
                    break;
                case GAME_ERR_OCCUPIED:
                    printf("That square is already occupied!\n");
                    break;
                case GAME_ERR_OVER:
                    printf("The game is already finished.\n");
                    break;
                default:
                    printf("Unknown error code %d.\n", result);
                    break;
            }
        }
    }

    // Final state render and outcome declaration
    print_board(&game);
    printf("========================================\n");
    printf("               GAME OVER                \n");
    printf("========================================\n");

    int winner = get_winner(&game);
    if (winner == X_WINS) {
        printf("Winner: Player X!\n");
    } else if (winner == O_WINS) {
        printf("Winner: Player O!\n");
    } else {
        printf("Game ended with state code %d.\n", winner);
        return -1;
    }
    return 0;
}

int main(void) {
    return play();
}