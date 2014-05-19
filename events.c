/* events.c
Author: Joshua Prendergast */

#include "events.h"
#include "game.h"

gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    /* Redraw the stave */
    game *g = (game *) data;
    GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state(widget)];

    gdk_pixbuf_render_to_drawable(g->stave, GDK_DRAWABLE(widget->window), gc, 0, 0, 0, 0, 250, 200, GDK_RGB_DITHER_MAX, 0, 0);
    
    int y = 162 - (g->note * 12);
    gdk_gc_set_rgb_fg_color(gc, &g->red);
    gdk_draw_arc(widget->window, gc, TRUE, 125, y, 21, 21, 0, 360 * 64); /* Draw the current note */

    return TRUE;
}

gboolean key_press_event_callback(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_Return) {
        /* Read the user's answer */
        GtkTextIter start, end;
        GtkTextBuffer *buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
        gtk_text_buffer_get_start_iter(buff, &start);
        gtk_text_buffer_get_end_iter(buff, &end);

        char *input = gtk_text_buffer_get_text(buff, &start, &end, FALSE);
        if (strlen(input) > 0) {
            gtk_text_buffer_delete(buff, &start, &end); /* Clear the buffer */
            game_submit_answer((game *) data, input);
        }
        return TRUE;
    }
    return FALSE;
}