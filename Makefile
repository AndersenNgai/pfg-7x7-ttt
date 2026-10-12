all: bin/ttt

bin:
	mkdir -p bin

# `make`
bin/ttt: bin src/main.c src/game.c include/game.h
	gcc -Wall -Wextra -O3 -std=c11 -Iinclude src/main.c src/game.c -o bin/ttt

# `make test`
test: bin bin/ttt_tests

bin/ttt_tests: src/game.c tests/game_tests.c include/game.h
	gcc -Wall -Wextra -O3 -std=c11 -Iinclude src/game.c tests/game_tests.c -lcriterion -o bin/ttt_tests

# `make clean`
clean:
	rm -rf bin

.PHONY: all test clean