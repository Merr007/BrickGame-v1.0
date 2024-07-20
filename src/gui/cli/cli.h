#ifndef CLI_H
#define CLI_H

#include <ncurses.h>

#include "../../brick_game/tetris/tetris.h"

void ncurses_init();
void colors_init();
void draw_field(WINDOW *win, GameInfo_t game_info);
void draw_score(WINDOW *win, GameInfo_t game_info);
void draw_highscore(WINDOW *win, GameInfo_t game_info);
void draw_level(WINDOW *win, GameInfo_t game_info);
void draw_next(WINDOW *win, GameInfo_t game_info);
void draw_pause(WINDOW *win);
void draw_gameover(WINDOW *win);
void draw_start(WINDOW *win);
void draw_instructions(WINDOW *win);
void print_components(GameInfo_t game_info);

#endif