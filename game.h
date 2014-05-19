/* game.h
Author: Joshua Prendergast */

#ifndef GAME_H
#define GAME_H

#include <gtk/gtk.h>

typedef struct {
    int note; // Current note; middle C is 0
    int correct;
    int total;
    GtkWidget *window; // Game window
    GtkWidget *drawing_area; // Image drawing area
    GtkWidget *input; // Text field
    GtkWidget *status;
    GtkWidget *layout; 
    GdkPixbuf *stave; // Stave image
    GdkColor red;
} game;

void game_init(game *game);

char *game_get_note_name(int note);

void game_next_note(game *game);

void game_submit_answer(game *game, char *answer);

void game_load_images(game *game);

#endif // GAME_H
