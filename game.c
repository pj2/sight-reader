/* game.c
Author: Joshua Prendergast */

#include "game.h"
#include "util.h"
#include "events.h"
#include <string.h>
#include <stdlib.h>

void game_init(State *state) {
    state->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    state->drawing_area = gtk_drawing_area_new();
    state->input = gtk_text_view_new();
    state->layout = gtk_vbox_new(FALSE, 0);
    state->status = gtk_label_new("Name the note on the stave");
    state->correct = state->total = 0;

    gtk_window_set_title(GTK_WINDOW(state->window), "sight-reader");
    gtk_window_set_position(GTK_WINDOW(state->window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(state->window), 250, 240);

    /* Setup our signal handlers */
    g_signal_connect(G_OBJECT(state->drawing_area), "expose_event", G_CALLBACK(expose_event_callback), (gpointer) state);
    g_signal_connect(G_OBJECT(state->window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(state->window));
    g_signal_connect(G_OBJECT(state->input), "key-press-event", G_CALLBACK(key_press_event_callback), (gpointer) state);

    gtk_container_add(GTK_CONTAINER(state->window), state->layout);
    gtk_box_pack_start(GTK_BOX(state->layout), state->drawing_area, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(state->layout), state->input, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(state->layout), state->status, FALSE, FALSE, 0);

    srand(time(NULL));
    game_next_note(state);

    game_load_images(state);
}

char *game_get_note_name(int note) {
    static char *names[] = {"C", "D", "E", "F", "G", "A", "B"};
    return names[abs(note) % 7];
}

void game_next_note(State *state) {
    state->note = 1 + (rand() % 14);
}

void game_submit_answer(State *state, char *answer) {
    static char text[50];
    strupper(answer);

    char *expected = game_get_note_name(state->note);
    if (strcmp(answer, expected) == 0)
        state->correct++;
    state->total++;

    game_next_note(state);
    gtk_widget_queue_draw(state->drawing_area);

    snprintf(text, sizeof(text), "Note was '%s' | Score: %d of %d", expected, state->correct, state->total);
    gtk_label_set_text(GTK_LABEL(state->status), text);
}

void game_load_images(State *state) {
    GError *err = NULL;
    if (!(state->stave = gdk_pixbuf_new_from_file("images/treble.png", &err)))
        printf(err->message);
}