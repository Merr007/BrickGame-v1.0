CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra
TESTLIBS = -lcheck -lm -lncurses
CPP = cppcheck --enable=all --suppress=missingIncludeSystem
TESTS = $(wildcard ./tests/*.c)
TARGET = tetris
INSTALL_DIR = ./bin
CLANG_FORMAT = ../materials/linters/.clang-format
UNAME_S := $(shell uname -s)
BROWSER_CMD = $(shell which xdg-open || which open || which firefox)

CFILES_BACK = $(shell find brick_game -name '*.c')
OBJS_BACK = $(CFILES_BACK:%.c=%.o)

CFILES_FRONT = $(shell find gui -name '*.c')
OBJS_FRONT = $(CFILES_FRONT:%.c=%.o)

CFILES_TEST = $(shell find tests -name '*.c')
OBJS_TEST = $(CFILES_TEST:%.c=%.o)

ifeq ($(UNAME_S),Linux)
  OPEN_CMD = xdg-open
	ADD_LIB = -lcheck -lsubunit -lm -lrt -lpthread -D_GNU_SOURCE
	BREW = .
endif
ifeq ($(UNAME_S),Darwin)
	OPEN_CMD = open
	ADD_LIB =
	BREW := ${HOME}/homebrew
endif

PATH := $(BREW)/bin:$(PATH)

.PHONY: all
all: tetris clean

.PHONY: install
install: tetris
	mkdir $(INSTALL_DIR)
	cp tetris $(INSTALL_DIR)
	@echo 0 > ./highscore.txt

.PHONY: uninstall
uninstall: clean
	rm -rf $(INSTALL_DIR)
	rm ./highscore.txt

.PHONY: run
run: tetris
	./bin/tetris
	
tetris: tetris.a $(OBJS_FRONT) tetris.o
	$(CC) $(CFLAGS) $^ -o $@ -lncurses $(ADD_LIB)

$(OBJS_BACK): %.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_FRONT): %.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_TEST): %.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

tetris.a: $(OBJS_BACK)
	ar rc tetris.a $(OBJS_BACK)
	ranlib tetris.a

.PHONY: test
test: 
	$(CC) $(CFLAGS) $(TESTS) -c
	$(CC) $(CFLAGS) -o test *.o ./brick_game/tetris/*.c --coverage $(TESTLIBS)
	chmod +x test
	./test
	make clean
		

gcov_report: ${SOURCE}
	gcc --coverage ${CFILES_BACK} $(TESTS) -o gcov_test -lcheck
	chmod +x gcov_test
	./gcov_test
	lcov -t "gcov_test" -o s21_test.info --no-external -c -d .
	genhtml -o report s21_test.info
	$(OPEN_CMD) ./report/index.html

lcov:
ifeq ("", "$(shell PATH=$(PATH) which lcov)")
	$(error Need to install lcov)
endif

.PHONY: dvi
dvi:
	doxygen
	$(BROWSER_CMD) doc/html/index.html

cppcheck:
		$(CPP) ./tetris.c ./brick_game/tetris/*.h ./brick_game/tetris/*.c ./gui/cli/*.c ./gui/cli/*.h ./tests/*.h ./tests/*.c

valgrind: test
	valgrind --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all -s  test

format:
	cp $(CLANG_FORMAT) .
	clang-format -n ./brick_game/tetris/*.h ./brick_game/tetris/*.c ./gui/cli/*.c ./gui/cli/*.h ./tests/*.h ./tests/*.c ./tetris.c
	rm .clang-format

formati:
	cp $(CLANG_FORMAT) .
	clang-format -i ./brick_game/tetris/*.h ./brick_game/tetris/*.c ./gui/cli/*.c ./gui/cli/*.h ./tests/*.h ./tests/*.c ./tetris.c
	rm .clang-format

.PHONY: dist
dist: tetris
	tar -cvzf BricksGame-1.tar.gz tetris ./Tetris.png
	rm -f *.o *.a gui/cli/*.o brick_game/tetris/*.o ./tetris

.PHONY: clean
clean:
		rm -f *.o *.a gui/cli/*.o brick_game/tetris/*.o ./tetris ./gcov_test
		rm -rf ./doc
		rm -rf *.gcda *.info *.gcno *.c.gcov
		rm -rf *.out report/* test
		rm -rf BricksGame-1.tar.gz BricksGame-1/

rebuild: clean tetris