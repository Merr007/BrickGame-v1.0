#include "tests_back.h"

// BRICK_CONTAINER

START_TEST(container_default_lifecicle) {
  BricksContainer_t *container = create_bricks_container();

  ck_assert_ptr_nonnull(container);
  ck_assert_ptr_nonnull(container->get_current);
  ck_assert_ptr_nonnull(container->get_next_random);

  ck_assert_ptr_nonnull(container->bricks);
  ck_assert_int_eq(container->num_of_bricks, 7);

  ck_assert_ptr_null(container->get_current(NULL, 0));
  ck_assert_ptr_null(container->get_next_random(NULL));

  if (container->bricks) free(container->bricks);
  if (container) free(container);
  container = NULL;
}
END_TEST

START_TEST(container_provider) {
  TetrisGame_t *tetris = get_game();
  BricksContainer_t *container = tetris->bricks;
  tetris = get_game();
  BricksContainer_t *provided = tetris->bricks;

  ck_assert_ptr_eq(container, provided);

  free_game(tetris);
}
END_TEST

START_TEST(container_brick) {
  BricksContainer_t *container = create_bricks_container();
  ck_assert_int_ge(container->num_of_bricks, 0);

  Brick_t *brick = container->get_next_random(container);
  to_next_state(brick);
  to_prev_state(brick);

  for (int i = 0; i < 25; i++) {
    brick = container->get_next_random(container);
  }

  ck_assert_ptr_nonnull(brick);
  for (int next = 0; next < brick->num_of_states; next++) {
    to_next_state(brick);
  }
  ck_assert_int_eq(brick->current_state, 0);

  for (int prev = 0; prev < brick->num_of_states; prev++) {
    to_prev_state(brick);
  }
  ck_assert_int_eq(brick->current_state, 0);
  if (container->bricks) free(container->bricks);
  container->bricks = NULL;
  if (container) free(container);
  container = NULL;
}
END_TEST

// BRICK_CONTAINER

// FSM

START_TEST(tetris_fsm__ready) {
  TetrisGame_t *tetris = get_game();

  tetris->game_status = START;
  userInput(Start, false);
  ck_assert_int_eq(tetris->game_status, MOVE);

  tetris->game_status = START;
  userInput(Terminate, false);
  ck_assert_int_eq(tetris->game_status, TERMINATE);

  tetris->game_status = START;
  userInput(Left, false);
  userInput(Right, false);
  ck_assert_int_eq(tetris->game_status, START);

  free_game(tetris);
}
END_TEST

START_TEST(tetris_fsm__spawn) {
  TetrisGame_t *tetris = get_game();

  tetris->game_status = SPAWN;
  userInput(Pause, false);
  ck_assert_int_eq(tetris->game_status, PAUSE);
  userInput(Pause, false);
  ck_assert_int_eq(tetris->game_status, MOVE);

  tetris->game_status = SPAWN;
  userInput(Left, false);
  userInput(Right, false);
  ck_assert_int_eq(tetris->game_status, SPAWN);

  tetris->game_status = SPAWN;
  userInput(Terminate, false);
  ck_assert_int_eq(tetris->game_status, TERMINATE);
  free_game(tetris);
}
END_TEST

START_TEST(tetris_fsm__movement) {
  TetrisGame_t *tetris = get_game();

  tetris->game_status = MOVE;

  userInput(Start, false);
  ck_assert_int_eq(tetris->game_status, MOVE);

  userInput(Left, false);
  ck_assert_int_eq(tetris->game_status, MOVE);
  userInput(Right, false);
  ck_assert_int_eq(tetris->game_status, MOVE);
  userInput(Up, false);
  ck_assert_int_eq(tetris->game_status, MOVE);
  userInput(Down, false);
  ck_assert_int_eq(tetris->game_status, MOVE);
  userInput(Action, false);
  ck_assert_int_eq(tetris->game_status, MOVE);
  userInput(Pause, false);
  ck_assert_int_eq(tetris->game_status, PAUSE);
  userInput(Pause, false);
  ck_assert_int_eq(tetris->game_status, MOVE);
  userInput(Terminate, false);
  ck_assert_int_eq(tetris->game_status, TERMINATE);

  free_game(tetris);
}
END_TEST

START_TEST(tetris_fsm__pause) {
  TetrisGame_t *tetris = get_game();

  tetris->game_status = PAUSE;
  userInput(Pause, false);
  ck_assert_int_eq(tetris->game_status, PAUSE);
  userInput(Pause, false);
  ck_assert_int_eq(tetris->game_status, MOVE);

  tetris->game_status = PAUSE;
  userInput(Left, false);
  ck_assert_int_eq(tetris->game_status, PAUSE);

  tetris->game_status = PAUSE;
  userInput(Terminate, false);
  ck_assert_int_eq(tetris->game_status, TERMINATE);

  free_game(tetris);
}
END_TEST

START_TEST(tetris_fsm__gameover) {
  TetrisGame_t *tetris = get_game();

  tetris->game_status = GAMEOVER;
  userInput(Start, false);
  ck_assert_int_eq(tetris->game_status, MOVE);

  tetris->game_status = GAMEOVER;
  userInput(Left, false);
  userInput(Right, false);
  ck_assert_int_eq(tetris->game_status, GAMEOVER);

  tetris->game_status = GAMEOVER;
  userInput(Terminate, false);
  ck_assert_int_eq(tetris->game_status, TERMINATE);

  free_game(tetris);
}
END_TEST

// FSM

// GAME

START_TEST(tetris_default_case) {
  TetrisGame_t *tetris = get_game();

  ck_assert_ptr_nonnull(tetris);
  ck_assert_ptr_null(tetris->current);
  ck_assert_ptr_null(tetris->next);
  ck_assert_ptr_nonnull(tetris->game_info.field);
  ck_assert_ptr_nonnull(tetris->game_info.next);
  ck_assert_int_eq(tetris->game_info.pause, 0);
  ck_assert_int_eq(tetris->game_info.level, 1);
  ck_assert_int_eq(tetris->game_status, START);

  move_down(tetris);
  move_left(tetris);
  move_right(tetris);
  rotate_brick(tetris);

  start(tetris);
  ck_assert_int_eq(tetris->game_status, MOVE);
  ck_assert_ptr_nonnull(tetris->current);
  ck_assert_ptr_nonnull(tetris->next);

  pause_game(tetris);
  ck_assert_int_eq(tetris->game_info.pause, 1);
  pause_game(tetris);
  ck_assert_int_eq(tetris->game_info.pause, 0);

  while (tetris->game_status != ATTACH) {
    move_down(tetris);
  }
  ck_assert_int_eq(tetris->game_status, ATTACH);

  updateCurrentState();
  ck_assert_int_eq(tetris->game_status, MOVE);

  free_game(tetris);
}
END_TEST

START_TEST(tetris_timer_is_ticked) {
  TetrisGame_t *tetris = get_game();

  start(tetris);
  int last_y = tetris->current->pos_y;
  ck_assert_int_eq(tetris->game_status, MOVE);

  sleep(1);
  updateCurrentState();
  ck_assert_int_eq(tetris->current->pos_y, last_y + 1);

  free_game(tetris);
}
END_TEST

START_TEST(tetris_movement) {
  TetrisGame_t *tetris = get_game();

  start(tetris);
  ck_assert_int_eq(tetris->game_status, MOVE);
  for (size_t left_btn_presser = 0; left_btn_presser < 10; left_btn_presser++) {
    move_left(tetris);
  }
  for (size_t right_btn_presser = 0; right_btn_presser < 10;
       right_btn_presser++) {
    move_right(tetris);
  }

  for (size_t down_btn_presser = 0; down_btn_presser < 10; down_btn_presser++) {
    move_down(tetris);
  }

  terminate(tetris);
  ck_assert_int_eq(tetris->game_status, TERMINATE);

  free_game(tetris);
}
END_TEST

// END_TEST

START_TEST(tetris_erease_lines) {
  TetrisGame_t *tetris = get_game();

  start(tetris);
  int score = tetris->game_info.score;
  ck_assert_int_eq(score, 0);

  tetris->game_info.score = 100;
  ck_assert_int_gt(tetris->game_info.score, score);

  free_game(tetris);
}
END_TEST

Suite *suite_tests_back(void) {
  Suite *s = suite_create("tetris");
  TCase *tc_core = tcase_create("default");
  suite_add_tcase(s, tc_core);

  tcase_add_test(tc_core, container_default_lifecicle);
  tcase_add_test(tc_core, container_provider);
  tcase_add_test(tc_core, container_brick);

  tcase_add_test(tc_core, tetris_fsm__ready);
  tcase_add_test(tc_core, tetris_fsm__spawn);
  tcase_add_test(tc_core, tetris_fsm__movement);
  tcase_add_test(tc_core, tetris_fsm__pause);
  tcase_add_test(tc_core, tetris_fsm__gameover);

  tcase_add_test(tc_core, tetris_default_case);
  tcase_add_test(tc_core, tetris_timer_is_ticked);
  tcase_add_test(tc_core, tetris_movement);
  tcase_add_test(tc_core, tetris_erease_lines);

  return s;
}

// GAME
