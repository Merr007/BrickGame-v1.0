#include "cli.h"

void ncurses_init() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  wtimeout(stdscr, 100);
  curs_set(0);
  refresh();
}

void colors_init() {
  start_color();
  init_color(COLOR_ORANGE, 996, 339, 199);
  init_color(COLOR_VIOLET, 292, 0, 507);
  init_pair(Red, COLOR_BLACK, COLOR_RED);
  init_pair(Orange, COLOR_BLACK, COLOR_ORANGE);
  init_pair(Yellow, COLOR_BLACK, COLOR_YELLOW);
  init_pair(Purple, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(Green, COLOR_BLACK, COLOR_GREEN);
  init_pair(Blue, COLOR_BLACK, COLOR_BLUE);
  init_pair(Violet, COLOR_BLACK, COLOR_VIOLET);
}

void print_components(GameInfo_t game_info) {
  draw_field(stdscr, game_info);
  draw_score(stdscr, game_info);
  draw_highscore(stdscr, game_info);
  draw_level(stdscr, game_info);
  draw_next(stdscr, game_info);
  draw_instructions(stdscr);
}

void draw_field(WINDOW *win, GameInfo_t game_info) {
  WINDOW *field = derwin(win, FIELD_ROWS + 2, (FIELD_COLS * 2) + 2, FIELD_POS_Y,
                         FIELD_POS_X);
  box(field, 0, 0);

  for (int row = 0; row < FIELD_ROWS; row++) {
    for (int col = 0; col < FIELD_COLS; col++) {
      int attr = game_info.field[row][col]
                     ? COLOR_PAIR(game_info.field[row][col])
                     : WA_DIM;
      wattron(field, attr);
      mvwprintw(field, row + 1, (col * 2) + 1, "%s",
                game_info.field[row][col] ? "  " : "");
      wattroff(field, attr);
    }
  }

  wnoutrefresh(field);
  delwin(field);
  field = NULL;
}

void draw_score(WINDOW *win, GameInfo_t game_info) {
  WINDOW *score_win =
      derwin(win, STATS_HEIGHT, STATS_WIDTH, FIELD_POS_Y + 2, SCORE_POS_X);
  box(score_win, 0, 0);

  wattron(score_win, WA_BOLD);
  mvwprintw(score_win, 0, (STATS_WIDTH - strlen("score")) / 2, "%s", "Score");

  char str[STATS_WIDTH];
  snprintf(str, STATS_WIDTH, "%d", game_info.score);
  mvwprintw(score_win, 1, (STATS_WIDTH - strlen(str)) / 2, "%d",
            game_info.score);
  wattroff(score_win, WA_BOLD);

  wnoutrefresh(score_win);
  delwin(score_win);
  score_win = NULL;
}

void draw_highscore(WINDOW *win, GameInfo_t game_info) {
  WINDOW *highscore_win =
      derwin(win, STATS_HEIGHT, STATS_WIDTH, FIELD_POS_Y + 6, SCORE_POS_X);
  box(highscore_win, 0, 0);

  wattron(highscore_win, WA_BOLD);
  mvwprintw(highscore_win, 0, (STATS_WIDTH - strlen("highscore")) / 2, "%s",
            "Highscore");

  char str[STATS_WIDTH];
  snprintf(str, STATS_WIDTH, "%d", game_info.high_score);
  mvwprintw(highscore_win, 1, (STATS_WIDTH - strlen(str)) / 2, "%d",
            game_info.high_score);
  wattroff(highscore_win, WA_BOLD);

  wnoutrefresh(highscore_win);
  delwin(highscore_win);
  highscore_win = NULL;
}
void draw_level(WINDOW *win, GameInfo_t game_info) {
  WINDOW *lvl_win =
      derwin(win, STATS_HEIGHT, STATS_WIDTH, FIELD_POS_Y + 10, SCORE_POS_X);
  box(lvl_win, 0, 0);

  wattron(lvl_win, WA_BOLD);
  mvwprintw(lvl_win, 0, ((STATS_WIDTH - strlen("level")) / 2), "%s", "Level");

  char str[STATS_WIDTH];
  snprintf(str, STATS_WIDTH, "%d", game_info.level);
  mvwprintw(lvl_win, 1, (STATS_WIDTH - strlen(str)) / 2, "%d", game_info.level);
  wattroff(lvl_win, WA_BOLD);

  wnoutrefresh(lvl_win);
  delwin(lvl_win);
  lvl_win = NULL;
}
void draw_next(WINDOW *win, GameInfo_t game_info) {
  WINDOW *next_win =
      derwin(win, NEXT_HEIGHT, STATS_WIDTH, FIELD_POS_Y + 14, SCORE_POS_X);
  box(next_win, 0, 0);

  wattron(next_win, WA_BOLD);
  mvwprintw(next_win, 0, (STATS_WIDTH - strlen("Next")) / 2, "%s", "Next");
  wattroff(next_win, WA_BOLD);

  for (int row = 0; row < BRICK_SIZE; row++) {
    for (int col = 0; col < BRICK_SIZE; col++) {
      int attr = game_info.next[row][col] ? COLOR_PAIR(game_info.next[row][col])
                                          : WA_DIM;
      wattron(next_win, attr);
      mvwprintw(next_win, row + 1, (col * 2) + 2, "%s",
                game_info.next[row][col] ? "  " : "");
      wattroff(next_win, attr);
    }
  }

  wnoutrefresh(next_win);
  delwin(next_win);
  next_win = NULL;
}

void draw_instructions(WINDOW *win) {
  WINDOW *instructions =
      derwin(win, 14, (FIELD_COLS * 2) + 3, FIELD_POS_Y + 4, SCORE_POS_X + 16);

  wattron(instructions, WA_BOLD);
  mvwprintw(instructions, 0,
            ((FIELD_COLS * 2) + 3 - strlen("Control Buttons")) / 2, "%s",
            "Control Buttons");
  mvwprintw(instructions, 2, 0, "%s", "P - Pause");
  mvwprintw(instructions, 4, 0, "%s", "Q - Exit");
  mvwprintw(instructions, 6, 0, "%s", "Key left - Move left");
  mvwprintw(instructions, 8, 0, "%s", "Key right - Move right");
  mvwprintw(instructions, 10, 0, "%s", "Key down - Move down");
  mvwprintw(instructions, 12, 0, "%s", "R - Rotate figure");
  wattroff(instructions, WA_BOLD);
  wnoutrefresh(instructions);
  delwin(instructions);
  instructions = NULL;
}

void draw_pause(WINDOW *field) {
  WINDOW *pause =
      derwin(field, 4, (FIELD_COLS * 2) + 2, FIELD_POS_Y - 4, FIELD_POS_X);

  wattron(pause, WA_BOLD);
  mvwprintw(pause, 0, ((FIELD_COLS * 2) + 2 - strlen("Pause")) / 2, "%s",
            "Pause");
  mvwprintw(pause, 1, ((FIELD_COLS * 2) + 2 - strlen("Press P to unpause")) / 2,
            "%s", "Press P to unpause");
  wattroff(pause, WA_BOLD);
  wnoutrefresh(pause);
  delwin(pause);
  pause = NULL;
}

void draw_gameover(WINDOW *field) {
  WINDOW *gameover =
      derwin(field, 4, (FIELD_COLS * 2) + 2, FIELD_POS_Y - 4, FIELD_POS_X);

  wattron(gameover, WA_BOLD);
  mvwprintw(gameover, 0, ((FIELD_COLS * 2) + 2 - strlen("Game Over")) / 2, "%s",
            "Game Over");
  mvwprintw(gameover, 1,
            ((FIELD_COLS * 2) + 2 - strlen("Press Enter to Restart")) / 2, "%s",
            "Press Enter to Restart");
  mvwprintw(gameover, 2, ((FIELD_COLS * 2) + 2 - strlen("Q to exit")) / 2, "%s",
            "Q to Exit");
  wattroff(gameover, WA_BOLD);
  wnoutrefresh(gameover);
  delwin(gameover);
  gameover = NULL;
}

void draw_start(WINDOW *win) {
  WINDOW *start =
      derwin(win, 4, (FIELD_COLS * 2) + 10, FIELD_POS_Y - 4, FIELD_POS_X);

  wattron(start, WA_BOLD);
  mvwprintw(start, 0,
            ((FIELD_COLS * 2) + 2 - strlen("Press Enter to Start")) / 2, "%s",
            "Press Enter to Start");
  wattroff(start, WA_BOLD);
  wnoutrefresh(start);
  delwin(start);
  start = NULL;
}