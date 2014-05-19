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
} State;

void game_init(State *state);

char *game_get_note_name(int note);

void game_next_note(State *state);

void game_submit_answer(State *state, char *answer);

void game_load_images(State *state);

#endif // GAME_H
