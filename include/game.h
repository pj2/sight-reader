/* game.h
Author: Joshua Prendergast */

#ifndef GAME_H
#define GAME_H

#include <gtk/gtk.h>
#include "main_window.h"
#include "note.h"

typedef struct game_s {
    note note;
    int correct; // Total correct
    int total; // Total answered
    int clef;
    int next_clef_swap; // Turns before the clef is next swapped
    main_window main_window;
} game;

void game_init(game *game);

void game_next_note(game *game);

void game_submit_answer(game *game, note *answer);

void game_load_images(game *game);

#endif // GAME_H
