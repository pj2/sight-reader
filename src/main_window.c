/* window.c
Author: Joshua Prendergast */

#include "main_window.h"
#include "events.h"

void main_window_init(game *game, main_window *window) {
    window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    window->drawing_area = gtk_drawing_area_new();
    window->input = gtk_text_view_new();
    window->layout = gtk_vbox_new(FALSE, 0);
    window->status = gtk_label_new("Name the note on the stave");

    gtk_window_set_title(GTK_WINDOW(window->window), "sight-reader");
    gtk_window_set_position(GTK_WINDOW(window->window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window->window), 250, 350);

    /* Setup our signal handlers */
    g_signal_connect(G_OBJECT(window->drawing_area), "expose_event", G_CALLBACK(expose_event_callback), (gpointer) game);
    g_signal_connect(G_OBJECT(window->window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window->window));
    g_signal_connect(G_OBJECT(window->input), "key-press-event", G_CALLBACK(key_press_event_callback), (gpointer) game);

    gtk_container_add(GTK_CONTAINER(window->window), window->layout);
    gtk_box_pack_start(GTK_BOX(window->layout), window->drawing_area, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(window->layout), window->input, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(window->layout), window->status, FALSE, FALSE, 0);

    main_window_load_images(window);
}

void main_window_load_images(main_window *window) {
    GError *err = NULL;
    if (!(window->treble = gdk_pixbuf_new_from_file("images/treble.png", &err)))
        printf(err->message);
    if (!(window->bass = gdk_pixbuf_new_from_file("images/bass.png", &err)))
        printf(err->message);
}
