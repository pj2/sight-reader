/* game.c
Author: Joshua Prendergast */

#include "game.h"
#include "util.h"
#include "events.h"
#include <string.h>
#include <stdlib.h>

void game_init(game *game) {
    game->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    game->drawing_area = gtk_drawing_area_new();
    game->input = gtk_text_view_new();
    game->layout = gtk_vbox_new(FALSE, 0);
    game->status = gtk_label_new("Name the note on the stave");
    game->correct = game->total = 0;

    gtk_window_set_title(GTK_WINDOW(game->window), "sight-reader");
    gtk_window_set_position(GTK_WINDOW(game->window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(game->window), 250, 240);

    /* Setup our signal handlers */
    g_signal_connect(G_OBJECT(game->drawing_area), "expose_event", G_CALLBACK(expose_event_callback), (gpointer) game);
    g_signal_connect(G_OBJECT(game->window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(game->window));
    g_signal_connect(G_OBJECT(game->input), "key-press-event", G_CALLBACK(key_press_event_callback), (gpointer) game);

    gtk_container_add(GTK_CONTAINER(game->window), game->layout);
    gtk_box_pack_start(GTK_BOX(game->layout), game->drawing_area, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(game->layout), game->input, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(game->layout), game->status, FALSE, FALSE, 0);

    srand(time(NULL));
    game_next_note(game);

    game_load_images(game);
}

char *game_get_note_name(int note) {
    static char *names[] = {"C", "D", "E", "F", "G", "A", "B"};
    return names[abs(note) % 7];
}

void game_next_note(game *game) {
    game->note = 1 + (rand() % 14);
}

void game_submit_answer(game *game, char *answer) {
    static char text[50];
    strupper(answer);

    char *expected = game_get_note_name(game->note);
    if (strcmp(answer, expected) == 0)
        game->correct++;
    game->total++;

    game_next_note(game);
    gtk_widget_queue_draw(game->drawing_area);

    snprintf(text, sizeof(text), "Note was '%s' | Score: %d of %d", expected, game->correct, game->total);
    gtk_label_set_text(GTK_LABEL(game->status), text);
}

void game_load_images(game *state) {
    GError *err = NULL;
    if (!(state->stave = gdk_pixbuf_new_from_file("images/treble.png", &err)))
        printf(err->message);
}