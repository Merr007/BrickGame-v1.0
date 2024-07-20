#ifndef TETRIS_H
#define TETRIS_H

#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"
#include "timer.h"

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum {
  START,
  SPAWN,
  MOVE,
  ATTACH,
  GAMEOVER,
  PAUSE,
  TERMINATE
} GameState_t;

typedef enum {
  Red = 1,
  Orange,
  Yellow,
  Purple,
  Green,
  Blue,
  Violet
} BrickColors;

typedef struct {
  int pos_x;
  int pos_y;
  int color;

  int current_state;
  int num_of_states;
  int states[4][BRICK_SIZE][BRICK_SIZE];
} Brick_t;

typedef struct BricksContainer_t {
  Brick_t *bricks;
  int num_of_bricks;

  Brick_t *(*get_current)(struct BricksContainer_t *curr, int index);
  Brick_t *(*get_next_random)(struct BricksContainer_t *curr);
} BricksContainer_t;

typedef struct {
  GameInfo_t game_info;
  BricksContainer_t *bricks;
  GameState_t game_status;
  Timer_t timer;
  Brick_t *current;
  Brick_t *next;
} TetrisGame_t;

// Input from gui
void userInput(UserAction_t action, bool hold);

// Current game status
GameInfo_t updateCurrentState();

// Utils
int **create_matrix(int rows, int cols);
void free_matrix(int **matrix, int rows);
TetrisGame_t *new_game();
TetrisGame_t *get_game();
void game_over(TetrisGame_t *tetris);
void free_game(TetrisGame_t *tetris);
int calculate_level(int score);
double calculate_timeout(TetrisGame_t *tetris);
void calculate_score(TetrisGame_t *tetris, int num_of_lines);
int remove_lines(int **field);
void shift_removed(int **field, int row);
int read_score_from_file();
void write_score_to_file(int score);
void empty_field(int **matrix);
void put_brick(TetrisGame_t *tetris);
void remove_brick(TetrisGame_t *tetris);
void to_next_state(Brick_t *brick);
void to_prev_state(Brick_t *brick);

// Bricks container
BricksContainer_t *create_bricks_container();
void add_brick(BricksContainer_t *bricks_container, Brick_t brick);
Brick_t *get_current_brick(struct BricksContainer_t *curr, int index);
Brick_t *get_next_random(struct BricksContainer_t *curr);

// Game operations
void move_down(TetrisGame_t *tetris);
void move_left(TetrisGame_t *tetris);
void move_right(TetrisGame_t *tetris);
void rotate_brick(TetrisGame_t *tetris);
void pause_game(TetrisGame_t *tetris);
void start(TetrisGame_t *tetris);
void spawn(TetrisGame_t *tetris);
void terminate(TetrisGame_t *tetris);

bool check_attach(TetrisGame_t *tetris);

#endif
