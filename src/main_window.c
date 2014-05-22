/* window.c
Author: Joshua Prendergast */

#include "main_window.h"

#define INT16_MAX 65535

void main_window_init(game *game, main_window *window) {
    window->game = game;
    window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    window->drawing_area = gtk_drawing_area_new();

    GtkWidget *layout = gtk_vbox_new(FALSE, 0);
    GtkWidget *controls = main_window_create_controls(window);

    gtk_window_set_title(GTK_WINDOW(window->window), "SightReader");
    gtk_window_set_position(GTK_WINDOW(window->window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window->window), 550, 450);

    /* Setup our signal handlers */
    g_signal_connect(G_OBJECT(window->drawing_area), "expose_event", G_CALLBACK(main_window_stave_exposed), (gpointer) window);
    g_signal_connect(G_OBJECT(window->window), "destroy", G_CALLBACK(game_destroy), (gpointer) game);

    gtk_container_add(GTK_CONTAINER(window->window), layout);
    gtk_box_pack_start(GTK_BOX(layout), window->drawing_area, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(layout), controls, FALSE, FALSE, 5);

    main_window_load_images(window);

    /* Initialize colors */
    window->red.red = INT16_MAX * 0.7;
    window->red.green = window->red.blue = 0;
    window->white.red = window->white.green = window->white.blue = INT16_MAX;
    window->black.red = window->black.green = window->black.blue = 0;
    window->gray.red = window->gray.green = window->gray.blue = INT16_MAX * 0.10;

    window->selected_modifier = NOTE_MOD_NONE;
}

void main_window_destroy(main_window *window) {
}

GtkWidget *main_window_create_controls(main_window *window) {
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0); /* Vertical layout; contains everything */
    GtkWidget *row1 = gtk_hbox_new(FALSE, 0); /* Horizontal layout for note buttons */

    /* Add note selection buttons */
    GtkWidget *cur_btn;

    int i;
    for (i = 0; i < NOTE_MAX; i++) {
        cur_btn = gtk_button_new();
        window->btns[i] = cur_btn;

        window->control_data[i].value = i;
        window->control_data[i].window = window;
        g_signal_connect(G_OBJECT(cur_btn), "clicked", G_CALLBACK(main_window_note_clicked), (gpointer) &window->control_data[i]);

        gtk_box_pack_start(GTK_BOX(row1), cur_btn, TRUE, TRUE, 1);
    }
    main_window_relabel_notes(window);

    /* Add rows */
    gtk_box_pack_start(GTK_BOX(vbox), row1, FALSE, FALSE, 0);

    /* Finally, add status line */
    window->status = gtk_label_new("Name the note on the stave");
    gtk_box_pack_start(GTK_BOX(vbox), window->status, FALSE, FALSE, 15);

    return vbox;
}

void main_window_load_images(main_window *window) {
    /* TODO error handling */
    GError *err = NULL;
    window->pixbufs[PIXBUF_TREBLE] = gdk_pixbuf_new_from_file("images/treble.png", &err);
    window->pixbufs[PIXBUF_BASS]   = gdk_pixbuf_new_from_file("images/bass.png"  , &err);
    window->pixbufs[PIXBUF_SHARP]  = gdk_pixbuf_new_from_file("images/sharp.png" , &err);
    window->pixbufs[PIXBUF_FLAT]   = gdk_pixbuf_new_from_file("images/flat.png"  , &err);
}

void main_window_relabel_notes(main_window *window) {
    char label[NOTE_NAME_MAX_LEN];
    note note;
    note.modifiers = window->selected_modifier;

    int i;
    for (i = 0; i < NOTE_MAX; i++) {
        note.value = i;
        note_get_name(&note, label);

        gtk_button_set_label(GTK_BUTTON(window->btns[i]), label);
    }
}

void main_window_set_selected_modifier(main_window *window, int selected_modifier) {
    window->selected_modifier = selected_modifier;
    main_window_relabel_notes(window);
}

/*
 * Signal handlers 
 */

void main_window_note_clicked(GtkWidget *widget, gpointer data) {
    control_data *cd = ((control_data *) data);

    /* Submit the answer */
    note user_answer = { .value = cd->value, .modifiers = cd->window->selected_modifier };
    game_submit_answer(cd->window->game, &user_answer);

    main_window_relabel_notes(cd->window);
}

gboolean main_window_stave_exposed(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    game *g = ((main_window *) data)->game;
    GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state(widget)];

    int treble = g->clef == CLEF_TREBLE;
    GdkPixbuf *buf = treble ? g->main_window.pixbufs[PIXBUF_TREBLE] : g->main_window.pixbufs[PIXBUF_BASS]; /* The stave image; treble or bass */

    /* Used for centering - apply to all drawing operations */
    int origin_x = event->area.width / 2 - gdk_pixbuf_get_width(buf) / 2;
    int origin_y = event->area.height / 2 - gdk_pixbuf_get_height(buf) / 2;

    /* Draw a dark gray background */
    gdk_gc_set_rgb_fg_color(gc, &g->main_window.gray);
    gdk_draw_rectangle(GDK_DRAWABLE(widget->window),
            gc,
            TRUE,
            event->area.x,
            event->area.y,
            event->area.width,
            event->area.height);

    /* Draw the stave, centered vertically */
    gdk_pixbuf_render_to_drawable(buf,
            GDK_DRAWABLE(widget->window),
            gc,
            0, 0,
            origin_x, origin_y,
            -1, -1,
            GDK_RGB_DITHER_MAX, 0, 0);

    int i;
    for (i = 0; i < NOTES_PER_BAR; i++) {
        note *drawn = &g->notes[i];
        int render_pos = treble ? drawn->value : drawn->value - 2; /* The line to draw the note on */

        int note_x = origin_x + STAVE_NOTE_X + 75 * i;
        int note_y = origin_y + STAVE_C_Y - render_pos * STAVE_SPACING_Y;

        if (g->current_note == i)
            gdk_gc_set_rgb_fg_color(gc, &g->main_window.red);
        else
            gdk_gc_set_rgb_fg_color(gc, &g->main_window.black);

        /* Draw the current note */
        gdk_draw_arc(widget->window,
                gc,
                TRUE,
                note_x, note_y,
                STAVE_GAP_Y, STAVE_GAP_Y,
                0, 360 * 64);

        /* Draw accidental */
        int flat = (drawn->modifiers & NOTE_MOD_FLAT) == NOTE_MOD_FLAT;
        int sharp = (drawn->modifiers & NOTE_MOD_SHARP) == NOTE_MOD_SHARP;
        if (flat || sharp) {
            gdk_pixbuf_render_to_drawable(sharp ? g->main_window.pixbufs[PIXBUF_SHARP] : g->main_window.pixbufs[PIXBUF_FLAT],
                    GDK_DRAWABLE(widget->window),
                    gc,
                    0, 0,
                    note_x - 13, note_y - 2,
                    -1, -1,
                    GDK_RGB_DITHER_MAX, 0, 0);
        }

        /* Draw ledger lines */
        gdk_gc_set_rgb_fg_color(gc, &g->main_window.black);
        int below = render_pos <= 0;
        int above = render_pos >= 12;

        if (below || above) {
            /* Calculate the amount of ledger lines to draw and the direction to draw them in. */
            int cur_line;
            int start = below ? 0 : 12; /* Start position */
            int dir = below ? -2 : 2;
            int end = drawn->value; /* Last line which might have a ledger (if it's even) */

            if (!treble)
                end -= 2;

            for (cur_line = start; below ? cur_line >= end : cur_line <= end; cur_line += dir) {
                gdk_draw_rectangle(
                    GDK_DRAWABLE(widget->window),
                    gc,
                    TRUE,
                    note_x + STAVE_GAP_Y / 2 - STAVE_LEDGER_LEN / 2, origin_y + STAVE_C_Y - cur_line * STAVE_SPACING_Y + (STAVE_GAP_Y / 2),
                    STAVE_LEDGER_LEN,
                    4);
            }
        }

        /* Draw note names */
        char label[NOTE_NAME_MAX_LEN];
        note_get_name(drawn, label);

        if (i < g->current_note) {
            /* Draw label */
        }
    }

    return TRUE;
}

