all: bin/ttt

bin:
	mkdir -p bin

# 1) Run `make`: Creates bin/ttt using src/main.c, src/board.c, and src/game.c (Zero residual files)
bin/ttt: bin src/main.c src/board.c src/game.c include/board.h include/game.h
	gcc -Wall -Wextra -O3 -std=c11 -Iinclude src/main.c src/board.c src/game.c -o bin/ttt

# 2) Run `make test`: Creates bin/ttt_tests, ignoring src/main.c entirely (Zero residual files)
test: bin bin/ttt_tests

bin/ttt_tests: src/board.c src/game.c tests/board_tests.c tests/game_tests.c include/board.h include/game.h
	gcc -Wall -Wextra -O3 -std=c11 -Iinclude src/board.c src/game.c tests/board_tests.c tests/game_tests.c -lcriterion -o bin/ttt_tests

# 3) Run `make clean`: Removes the bin directory completely
clean:
	rm -rf bin

.PHONY: all test clean