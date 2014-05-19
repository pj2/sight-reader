/* main_window.h
Author: Joshua Prendergast */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtk.h>
#include "note.h"

#define STAVE_C_Y 162
#define STAVE_SPACING_Y 12
#define STAVE_NOTE_X 125
#define STAVE_GAP_Y 21
#define STAVE_LEDGER_LEN 35

typedef struct game_s game;
typedef struct main_window_s main_window;

typedef struct {
    int value; /* A uniquely identifing value */
    main_window *window;
} control_data;

typedef struct main_window_s {
    game *game;
    GtkWidget *window; // Game window
    GtkWidget *drawing_area; // Image drawing area
    GtkWidget *input; // Text field
    GtkWidget *status;
    GdkPixbuf *treble; // Treble clef stave image
    GdkPixbuf *bass; // Bass clef stave image
    GdkPixbuf *sharp;
    GdkPixbuf *flat;
    GdkColor red;
    GdkColor white;
    GdkColor black;
    int selected_modifier;
    control_data control_data[10];
} main_window;

#include "game.h"

void main_window_init(game *game, main_window *window);

GtkWidget *main_window_create_controls(main_window *window);

void main_window_load_images(main_window *window);

void main_window_modifier_clicked(GtkWidget *widget, gpointer data);

void main_window_note_clicked(GtkWidget *widget, gpointer data);

gboolean main_window_stave_exposed(GtkWidget *widget, GdkEventExpose *event, gpointer data);

#endif // MAINWINDOW_H
