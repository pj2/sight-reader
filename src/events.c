/* events.c
Author: Joshua Prendergast */

#include "events.h"
#include "game.h"
#include <string.h>
#include <math.h>

#define STAVE_C_Y 162
#define STAVE_SPACING_Y 12
#define STAVE_NOTE_X 125
#define STAVE_GAP_WIDTH 21
#define STAVE_LEDGER_LEN 35

gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    game *g = (game *) data;
    GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state(widget)];

    int treble = g->clef == CLEF_TREBLE;
    int real_pos = treble ? g->note : g->note - 2;

    /* Draw the stave */
    gdk_pixbuf_render_to_drawable(
        treble ? g->main_window.treble : g->main_window.bass,
        GDK_DRAWABLE(widget->window),
        gc,
        0,
        0,
        0,
        0,
        250,
        200,
        GDK_RGB_DITHER_MAX,
        0,
        0);

    /* Draw the current note in red */
    gdk_gc_set_rgb_fg_color(gc, &g->main_window.red);
    gdk_draw_arc(
        widget->window,
        gc,
        TRUE,
        STAVE_NOTE_X,
        STAVE_C_Y - real_pos * STAVE_SPACING_Y,
        STAVE_GAP_WIDTH,
        STAVE_GAP_WIDTH,
        0,
        360 * 64);

    /* Draw ledger lines */
    int below = real_pos <= 0;
    int above = real_pos >= 12;

    if (below || above) {
        /* Calculate the amount of ledger lines to draw, and the direction to draw them in. */
        int cur;
        int start = below ? 0 : 12; /* Start position */
        int dir = below ? -2 : 2;
        int end = g->note + dir / 2; /* Last line which might have a ledger (if it's even) */

        if (!treble)
            end += dir;

        for (cur = start; below ? cur >= end : cur <= end; cur += dir) {
            gdk_draw_rectangle(
                GDK_DRAWABLE(widget->window),
                gc,
                TRUE,
                STAVE_NOTE_X + STAVE_GAP_WIDTH / 2 - STAVE_LEDGER_LEN / 2,
                STAVE_C_Y - cur * STAVE_SPACING_Y + (STAVE_GAP_WIDTH / 2),
                STAVE_LEDGER_LEN,
                2);
        }
    }

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