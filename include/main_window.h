/* main_window.h
Author: Joshua Prendergast */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtk.h>
#include "note.h"

#define STAVE_C_Y 210
#define STAVE_SPACING_Y 12
#define STAVE_NOTE_X 125
#define STAVE_GAP_Y 21
#define STAVE_LEDGER_LEN 35

#define PIXBUF_COUNT 4
#define PIXBUF_TREBLE 0
#define PIXBUF_BASS 1
#define PIXBUF_SHARP 2
#define PIXBUF_FLAT 3

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
    GdkPixbuf *pixbufs[PIXBUF_COUNT];
    GtkWidget *btns[NOTE_MAX];
    GdkColor red;
    GdkColor white;
    GdkColor black;
    GdkColor gray;
    int selected_modifier;
    control_data control_data[3];
} main_window;

#include "game.h"

void main_window_init(game *game, main_window *window);

void main_window_destroy(main_window *window);

GtkWidget *main_window_create_controls(main_window *window);

void main_window_load_images(main_window *window);

void main_window_set_selected_modifier(main_window *window, int selected_modifier);

void main_window_relabel_notes(main_window *window);

void main_window_note_clicked(GtkWidget *widget, gpointer data);

gboolean main_window_stave_exposed(GtkWidget *widget, GdkEventExpose *event, gpointer data);

#endif // MAINWINDOW_H
