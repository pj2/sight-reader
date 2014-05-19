/* game.h
Author: Joshua Prendergast */

#ifndef GAME_H
#define GAME_H

#include <gtk/gtk.h>
#include "main_window.h"

#define CLEF_TREBLE 0
#define CLEF_BASS 1

typedef struct game_s {
    int note; // Current note; middle C is 0
    int correct; // Total correct
    int total; // Total answered
    int clef;
    int next_clef_swap; // Turns before the clef is next swapped
    main_window main_window;
} game;

void game_init(game *game);

char *game_get_note_name(int note);

void game_next_note(game *game);

void game_submit_answer(game *game, char *answer);

void game_load_images(game *game);

#endif // GAME_H
