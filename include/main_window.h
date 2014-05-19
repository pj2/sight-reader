/* main_window.h
Author: Joshua Prendergast */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtk.h>

typedef struct game_s game;

typedef struct {
    GtkWidget *window; // Game window
    GtkWidget *drawing_area; // Image drawing area
    GtkWidget *input; // Text field
    GtkWidget *status;
    GtkWidget *layout; 
    GdkPixbuf *treble; // Treble clef stave image
    GdkPixbuf *bass; // Bass clef stave image
    GdkColor red;
    GdkColor white;
} main_window;

void main_window_init(game *game, main_window *window);

void main_window_load_images(main_window *window);

#endif // MAINWINDOW_H
