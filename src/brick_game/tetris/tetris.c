#include "tetris.h"

/**
 * @brief Handles input from user based on current game state
 *
 * @param action Action provided by user
 * @param hold Continuous action
 */

void userInput(UserAction_t action, bool hold) {
  (void)hold;

  TetrisGame_t *tetris = get_game();
  switch (tetris->game_status) {
    case START:
      if (action == Start)
        start(tetris);
      else if (action == Terminate)
        terminate(tetris);
      break;
    case SPAWN:
      if (action == Pause)
        pause_game(tetris);
      else if (action == Terminate)
        terminate(tetris);
      break;
    case MOVE:
      if (action == Left)
        move_left(tetris);
      else if (action == Right)
        move_right(tetris);
      else if (action == Down)
        move_down(tetris);
      else if (action == Action)
        rotate_brick(tetris);
      else if (action == Pause)
        pause_game(tetris);
      else if (action == Terminate)
        terminate(tetris);
      break;
    case PAUSE:
      if (action == Pause)
        pause_game(tetris);
      else if (action == Terminate)
        terminate(tetris);
      break;
    case GAMEOVER:
      if (action == Start) start(tetris);
      if (action == Terminate) terminate(tetris);
      break;
    default:
      break;
  }
}

GameInfo_t updateCurrentState() {
  TetrisGame_t *tetris = get_game();
  tetris->game_info.level = calculate_level(tetris->game_info.score);
  tetris->timer.timeout = calculate_timeout(tetris);
  tetris->game_info.speed = tetris->timer.timeout * 1000;

  if (tetris->game_status == ATTACH) {
    calculate_score(tetris, remove_lines(tetris->game_info.field));
    if (tetris->game_info.score > tetris->game_info.high_score) {
      tetris->game_info.high_score = tetris->game_info.score;
    }
    spawn(tetris);
  } else if (check_timeout(&tetris->timer) && tetris->game_status == MOVE) {
    move_down(tetris);
  }
  return tetris->game_info;
}

int calculate_level(int score) {
  int level = score / SCORE_TO_LVL + 1;
  if (level > 10) level = 10;
  return level;
}

double calculate_timeout(TetrisGame_t *tetris) {
  double timeout = 0;
  timeout = TIME_DELAY_SEC * pow(0.8, tetris->game_info.level);
  return timeout;
}

void calculate_score(TetrisGame_t *tetris, int num_of_lines) {
  switch (num_of_lines) {
    case 1:
      tetris->game_info.score += 100;
      break;
    case 2:
      tetris->game_info.score += 300;
      break;
    case 3:
      tetris->game_info.score += 700;
      break;
    case 4:
      tetris->game_info.score += 1500;
      break;
    default:
      break;
  }
}

void start(TetrisGame_t *tetris) {
  if (tetris->game_status == START) {
    tetris->game_info.high_score = read_score_from_file();
    tetris->game_info.speed = tetris->timer.timeout * 1000;
  }

  if (tetris->game_status == GAMEOVER) {
    empty_field(tetris->game_info.field);
    tetris->game_info.score = 0;
    tetris->game_info.level = 1;
    tetris->game_info.pause = 0;
  }

  spawn(tetris);
}

void spawn(TetrisGame_t *tetris) {
  if (!tetris->next) {
    tetris->next = tetris->bricks->get_next_random(tetris->bricks);
  }

  tetris->current = tetris->next;
  tetris->next = tetris->bricks->get_next_random(tetris->bricks);

  for (int i = 0; i < BRICK_SIZE; i++) {
    for (int j = 0; j < BRICK_SIZE; j++) {
      if (tetris->next->states[tetris->next->current_state][i][j]) {
        tetris->game_info.next[i][j] = tetris->next->color;
      } else {
        tetris->game_info.next[i][j] = 0;
      }
    }
  }

  tetris->current->pos_x = SPAWN_POS_X;
  tetris->current->pos_y = SPAWN_POS_Y;

  if (!check_attach(tetris)) {
    put_brick(tetris);
    tetris->game_status = MOVE;
  } else {
    tetris->game_status = GAMEOVER;
    tetris->game_info.pause = -1;
  }
}

void move_down(TetrisGame_t *tetris) {
  Brick_t *curr = tetris->current;
  bool is_attached = false;

  if (curr) {
    remove_brick(tetris);
    curr->pos_y++;
    if (check_attach(tetris)) {
      is_attached = true;
      curr->pos_y--;
    }

    put_brick(tetris);
    if (is_attached) {
      tetris->game_status = ATTACH;
      tetris->current = NULL;
    }
  }
}

void move_left(TetrisGame_t *tetris) {
  Brick_t *curr = tetris->current;

  if (curr) {
    remove_brick(tetris);
    curr->pos_x--;
    if (check_attach(tetris)) {
      curr->pos_x++;
    }
    put_brick(tetris);
  }
}

void move_right(TetrisGame_t *tetris) {
  Brick_t *curr = tetris->current;

  if (curr) {
    remove_brick(tetris);
    curr->pos_x++;
    if (check_attach(tetris)) {
      curr->pos_x--;
    }
    put_brick(tetris);
  }
}

void pause_game(TetrisGame_t *tetris) {
  if (tetris->game_info.pause) {
    tetris->game_info.pause = 0;
    tetris->game_status = MOVE;
  } else {
    tetris->game_info.pause = 1;
    tetris->game_status = PAUSE;
  }
}

void terminate(TetrisGame_t *tetris) {
  tetris->game_status = TERMINATE;
  write_score_to_file(tetris->game_info.high_score);
}

void put_brick(TetrisGame_t *tetris) {
  int x_pos = tetris->current->pos_x;
  int y_pos = tetris->current->pos_y;
  Brick_t *curr = tetris->current;

  for (int i = 0; i < BRICK_SIZE; i++) {
    for (int j = 0; j < BRICK_SIZE; j++) {
      if (curr->states[curr->current_state][i][j]) {
        tetris->game_info.field[y_pos + (-HALF_BRICK_SIZE + 1) + i]
                               [x_pos - HALF_BRICK_SIZE + j] =
            tetris->current->color;
      }
    }
  }
}

void remove_brick(TetrisGame_t *tetris) {
  int x_pos = tetris->current->pos_x;
  int y_pos = tetris->current->pos_y;
  Brick_t *curr = tetris->current;

  for (int i = 0; i < BRICK_SIZE; i++) {
    for (int j = 0; j < BRICK_SIZE; j++) {
      if (curr->states[curr->current_state][i][j]) {
        tetris->game_info.field[y_pos + (-HALF_BRICK_SIZE + 1) + i]
                               [x_pos - HALF_BRICK_SIZE + j] = 0;
      }
    }
  }
}

int remove_lines(int **field) {
  int removed = 0, occupied = 0;
  for (int i = FIELD_ROWS - 1; i >= 0; i--) {
    for (int j = 0; j < FIELD_COLS; j++) {
      if (field[i][j]) occupied++;
    }

    if (occupied == FIELD_COLS) {
      shift_removed(field, i);
      removed++;
      i++;
    }
    occupied = 0;
  }
  return removed;
}

void shift_removed(int **field, int row) {
  while (row >= 0) {
    for (int i = 0; i < FIELD_COLS; i++) {
      if (row == 0)
        field[row][i] = 0;
      else
        field[row][i] = field[row - 1][i];
    }
    row--;
  }
}

void to_next_state(Brick_t *brick) {
  brick->current_state++;
  if (brick->current_state >= brick->num_of_states) brick->current_state = 0;
}

void to_prev_state(Brick_t *brick) {
  brick->current_state--;
  if (brick->current_state < 0) brick->current_state = brick->num_of_states - 1;
}

void rotate_brick(TetrisGame_t *tetris) {
  Brick_t *curr = tetris->current;
  if (curr) {
    remove_brick(tetris);
    to_next_state(curr);
    if (check_attach(tetris)) {
      to_prev_state(curr);
    }
    put_brick(tetris);
  }
}

bool check_attach(TetrisGame_t *tetris) {
  bool is_attached = false;

  int x_pos = tetris->current->pos_x;
  int y_pos = tetris->current->pos_y;
  Brick_t *curr = tetris->current;

  for (int i = 0; i < BRICK_SIZE && !is_attached; i++) {
    for (int j = 0; j < BRICK_SIZE && !is_attached; j++) {
      if (curr->states[curr->current_state][i][j]) {
        int x_pos_field = x_pos - HALF_BRICK_SIZE + j;
        int y_pos_field = y_pos + (-HALF_BRICK_SIZE + 1) + i;
        if ((x_pos_field < 0) || (x_pos_field >= FIELD_COLS))
          is_attached = true;
        else if ((y_pos_field < 0) || (y_pos_field >= FIELD_ROWS))
          is_attached = true;
        else {
          is_attached =
              tetris->game_info.field[y_pos_field][x_pos_field] ? true : false;
        }
      }
    }
  }
  return is_attached;
}

BricksContainer_t *create_bricks_container() {
  BricksContainer_t *bricks_container =
      (BricksContainer_t *)malloc(sizeof(BricksContainer_t));
  bricks_container->num_of_bricks = 0;
  bricks_container->bricks = NULL;
  bricks_container->get_current = get_current_brick;
  bricks_container->get_next_random = get_next_random;
  if (bricks_container) {
    // ****
    add_brick(
        bricks_container,
        (Brick_t){
            .pos_x = 0,
            .pos_y = 0,
            .color = Red,
            .current_state = 0,
            .num_of_states = 2,
            .states = {
                {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
                {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}}});
    // **
    // **
    add_brick(
        bricks_container,
        (Brick_t){
            .pos_x = 0,
            .pos_y = 0,
            .color = Purple,
            .current_state = 0,
            .num_of_states = 1,
            .states = {
                {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}}});
    //  **
    // **
    add_brick(
        bricks_container,
        (Brick_t){
            .pos_x = 0,
            .pos_y = 0,
            .color = Green,
            .current_state = 0,
            .num_of_states = 2,
            .states = {
                {{0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}},
                {{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}}}});
    // **
    //  **
    add_brick(
        bricks_container,
        (Brick_t){
            .pos_x = 0,
            .pos_y = 0,
            .color = Violet,
            .current_state = 0,
            .num_of_states = 2,
            .states = {
                {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},
                {{0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}}}});
    // ***
    // *
    add_brick(
        bricks_container,
        (Brick_t){
            .pos_x = 0,
            .pos_y = 0,
            .color = Orange,
            .current_state = 0,
            .num_of_states = 4,
            .states = {
                {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}},
                {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
                {{0, 0, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
                {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}}}});
    // ***
    //   *
    add_brick(
        bricks_container,
        (Brick_t){
            .pos_x = 0,
            .pos_y = 0,
            .color = Yellow,
            .current_state = 0,
            .num_of_states = 4,
            .states = {
                {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}},
                {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
                {{0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
                {{0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}}});
    //  *
    // ***
    add_brick(
        bricks_container,
        (Brick_t){
            .pos_x = 0,
            .pos_y = 0,
            .color = Blue,
            .current_state = 0,
            .num_of_states = 4,
            .states = {
                {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},
                {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
                {{0, 0, 1, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
                {{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}}}});
  }
  return bricks_container;
}

void add_brick(BricksContainer_t *bricks_container, Brick_t brick) {
  if (bricks_container) {
    bricks_container->bricks = (Brick_t *)realloc(
        bricks_container->bricks,
        sizeof(Brick_t) * (bricks_container->num_of_bricks + 1));
    bricks_container->bricks[bricks_container->num_of_bricks] = brick;
    bricks_container->num_of_bricks++;
  }
}

Brick_t *get_current_brick(BricksContainer_t *curr, int index) {
  if (!curr) return NULL;
  Brick_t *curr_brick = &curr->bricks[index];
  curr_brick->pos_x = 0;
  curr_brick->pos_y = 0;
  curr_brick->current_state = 0;

  return curr_brick;
}

Brick_t *get_next_random(BricksContainer_t *curr) {
  if (!curr) return NULL;
  int random_index = 0;
  static int last = -1;
  while (random_index == last) {
    random_index = rand() % curr->num_of_bricks;
  }
  last = random_index;
  Brick_t *random_brick = curr->get_current(curr, random_index);
  return random_brick;
}

TetrisGame_t *get_game() {
  static TetrisGame_t *tetris = NULL;
  if (!tetris) {
    tetris = new_game();
  }
  return tetris;
}

int read_score_from_file() {
  int high_score = 0;
  FILE *file = fopen(HIGHSCORE_FILE, "r");
  if (file) {
    fscanf(file, "%d", &high_score);
    fclose(file);
  }

  return high_score;
}

void write_score_to_file(int score) {
  FILE *file = fopen(HIGHSCORE_FILE, "w");
  if (file) {
    fprintf(file, "%d", score);
    fclose(file);
  }
}

TetrisGame_t *new_game() {
  TetrisGame_t *tetris = (TetrisGame_t *)malloc(sizeof(TetrisGame_t));
  if (tetris) {
    tetris->game_info =
        (GameInfo_t){.field = create_matrix(FIELD_ROWS, FIELD_COLS),
                     .next = create_matrix(BRICK_SIZE, BRICK_SIZE),
                     .level = 1,
                     .high_score = 0,
                     .pause = 0,
                     .score = 0,
                     .speed = 0};
    tetris->bricks = create_bricks_container();
    tetris->game_status = START;
    tetris->current = NULL;
    tetris->next = NULL;
    tetris->timer = new_timer(calculate_timeout(tetris));
  }
  return tetris;
}

int **create_matrix(int rows, int cols) {
  int **matrix = (int **)calloc(rows, sizeof(int *));
  for (int i = 0; i < rows; i++) {
    matrix[i] = (int *)calloc(cols, sizeof(int));
  }
  return matrix;
}

void empty_field(int **matrix) {
  for (int i = 0; i < FIELD_ROWS; i++) {
    for (int j = 0; j < FIELD_COLS; j++) {
      matrix[i][j] = 0;
    }
  }
}

void free_game(TetrisGame_t *tetris) {
  if (tetris) {
    free_matrix(tetris->game_info.field, FIELD_ROWS);
    free_matrix(tetris->game_info.next, BRICK_SIZE);
    if (tetris->bricks->bricks) {
      free(tetris->bricks->bricks);
      tetris->bricks->bricks = NULL;
    }

    if (tetris->bricks) {
      free(tetris->bricks);
      tetris->bricks = NULL;
    }

    tetris->current = NULL;
    tetris->next = NULL;
    free(tetris);
    tetris = NULL;
  }
}

void free_matrix(int **matrix, int rows) {
  if (matrix) {
    for (int i = 0; i < rows; i++) {
      free(matrix[i]);
    }
    free(matrix);
    matrix = NULL;
  }
}