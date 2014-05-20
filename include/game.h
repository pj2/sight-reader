/* game.h
Author: Joshua Prendergast */

#ifndef GAME_H
#define GAME_H

#include <gtk/gtk.h>
#include "main_window.h"
#include "note.h"

#define NOTES_PER_BAR 4

typedef struct game_s {
    note notes[NOTES_PER_BAR];
    int current_note;
    int correct; // Total correct
    int total; // Total answered
    int clef;
    int next_clef_swap; // Turns before the clef is next swapped
    main_window main_window;
} game;

void game_init(game *game);

void game_destroy(game *game);

void game_next_note(game *game, note *note);

void game_next_bar(game *game);

void game_submit_answer(game *game, note *answer);

void game_load_images(game *game);

#endif // GAME_H
