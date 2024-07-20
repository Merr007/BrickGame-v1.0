#include "brick_game/tetris/tetris.h"

#include "gui/cli/cli.h"

UserAction_t get_user_action(int input) {
  UserAction_t action = -1;
  if (input == ENTER_BUTTON) {
    action = Start;
  } else if (input == KEY_P) {
    action = Pause;
  } else if (input == KEY_Q) {
    action = Terminate;
  } else if (input == KEY_LEFT) {
    action = Left;
  } else if (input == KEY_RIGHT) {
    action = Right;
  } else if (input == KEY_DOWN) {
    action = Down;
  } else if (input == KEY_R) {
    action = Action;
  }

  return action;
}

void game_loop() {
  bool is_terminated = false;
  int input;
  TetrisGame_t *tetris = get_game();
  while (!is_terminated) {
    if (tetris->game_status == START) draw_start(stdscr);
    input = getch();
    userInput(get_user_action(input), false);
    wclear(stdscr);

    GameInfo_t game_info = updateCurrentState();
    print_components(game_info);

    if (tetris->game_status == PAUSE) {
      draw_pause(stdscr);
    } else if (tetris->game_status == GAMEOVER) {
      draw_gameover(stdscr);
    }

    doupdate();

    is_terminated = tetris->game_status == TERMINATE ? true : false;
  }

  free_game(tetris);
}

int main() {
  srand(time(NULL));
  setlocale(LC_ALL, "");
  ncurses_init();
  colors_init();
  game_loop();
  endwin();
  return 0;
}