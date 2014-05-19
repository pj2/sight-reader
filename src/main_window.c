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
    g_signal_connect(G_OBJECT(window->window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window->window));

    gtk_container_add(GTK_CONTAINER(window->window), layout);
    gtk_box_pack_start(GTK_BOX(layout), window->drawing_area, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(layout), controls, FALSE, FALSE, 5);

    main_window_load_images(window);

    /* Initialize colors */
    window->red.red = INT16_MAX * 0.7;
    window->red.green = window->red.blue = 0;
    window->white.red = window->white.green = window->white.blue = INT16_MAX;
    window->black.red = window->black.green = window->black.blue = 0;
}

GtkWidget *main_window_create_controls(main_window *window) {
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0); /* Vertical layout; contains everything */
    GtkWidget *row1 = gtk_hbox_new(FALSE, 0); /* Horizontal layout for note buttons */

    /* Add note selection buttons */
    GtkWidget *btn;
    char label[NOTE_NAME_MAX_LEN];

    note note;
    note.modifiers = NOTE_MOD_NONE;

    int i;
    for (i = 0; i < 7; i++) {
        note.value = i;
        note_get_name(&note, label);

        btn = gtk_button_new_with_label(label);

        window->control_data[i].value = i;
        window->control_data[i].window = window;
        g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(main_window_note_clicked), (gpointer) &window->control_data[i]);

        gtk_box_pack_start(GTK_BOX(row1), btn, TRUE, TRUE, 1);
    }

    /* Add modifier selection controls */
    GtkWidget *row2 = gtk_hbox_new(TRUE, 0);
    GtkWidget *radio[3];
    radio[0] = gtk_radio_button_new_with_label(NULL, "Natural");
    radio[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio[0]), "Sharp");
    radio[2] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio[0]), "Flat");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio[0]), TRUE);

    /* Set up the signals... */
    for (i = 0; i < 3; i++) {
        window->control_data[i + 7].value = i;
        window->control_data[i + 7].window = window;
        g_signal_connect(G_OBJECT(radio[i]), "clicked", G_CALLBACK(main_window_modifier_clicked), (gpointer) &window->control_data[i + 7]);
    }

    gtk_box_pack_start(GTK_BOX(row2), radio[0], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row2), radio[1], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row2), radio[2], FALSE, FALSE, 0);

    /* Add rows */
    gtk_box_pack_start(GTK_BOX(vbox), row1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), row2, FALSE, FALSE, 0);

    /* Finally, add status line */
    window->status = gtk_label_new("Name the note on the stave");
    gtk_box_pack_start(GTK_BOX(vbox), window->status, FALSE, FALSE, 15);

    return vbox;
}

void main_window_load_images(main_window *window) {
    GError *err = NULL;
    if (!(window->treble = gdk_pixbuf_new_from_file("images/treble.png", &err)) ||
        !(window->bass   = gdk_pixbuf_new_from_file("images/bass.png"  , &err)) ||
        !(window->sharp  = gdk_pixbuf_new_from_file("images/sharp.png" , &err)) ||
        !(window->flat   = gdk_pixbuf_new_from_file("images/flat.png"  , &err))) {
        printf("%s\n", err->message);
    }
}

/*
 * Signal handlers 
 */

void main_window_modifier_clicked(GtkWidget *widget, gpointer data) {
    /* Update the selected modifier. This struct passing seems convoluted, but I can't think of a better alternative right now */
    control_data *cd = ((control_data *) data);
    cd->window->selected_modifier = cd->value;
}

void main_window_note_clicked(GtkWidget *widget, gpointer data) {
    control_data *cd = ((control_data *) data);

    /* Submit the answer */
    note user_answer = { .value = cd->value, .modifiers = cd->window->selected_modifier };
    game_submit_answer(cd->window->game, &user_answer);
}

gboolean main_window_stave_exposed(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    game *g = ((main_window *) data)->game;
    GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state(widget)];

    int treble = g->clef == CLEF_TREBLE;
    GdkPixbuf *buf = treble ? g->main_window.treble : g->main_window.bass; /* The stave image; treble or bass */
    int origin_y = event->area.height / 2 - gdk_pixbuf_get_height(buf) / 2; /* Used for centering - apply to all drawing operations */

    /* Draw a white background */
    gdk_gc_set_rgb_fg_color(gc, &g->main_window.white);
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
            event->area.x, origin_y,
            -1, -1,
            GDK_RGB_DITHER_MAX, 0, 0);

    note *current_note = &g->note;
    int render_pos = treble ? current_note->value : current_note->value - 2; /* The line to draw the note on */

    int note_x = STAVE_NOTE_X;
    int note_y = origin_y + STAVE_C_Y - render_pos * STAVE_SPACING_Y;

    /* Draw the current note in red */
    gdk_gc_set_rgb_fg_color(gc, &g->main_window.red);
    gdk_draw_arc(widget->window,
            gc,
            TRUE,
            note_x, note_y,
            STAVE_GAP_Y, STAVE_GAP_Y,
            0, 360 * 64);

    /* Draw accidental */
    int flat = (current_note->modifiers & NOTE_MOD_FLAT) == NOTE_MOD_FLAT;
    int sharp = (current_note->modifiers & NOTE_MOD_SHARP) == NOTE_MOD_SHARP;
    if (flat || sharp) {
        gdk_pixbuf_render_to_drawable(sharp ? g->main_window.sharp : g->main_window.flat,
                GDK_DRAWABLE(widget->window),
                gc,
                0, 0,
                note_x + 10, note_y - 5,
                -1, -1,
                GDK_RGB_DITHER_MAX, 0, 0);
    }

    /* Draw ledger lines */
    gdk_gc_set_rgb_fg_color(gc, &g->main_window.black);
    int below = render_pos <= 0;
    int above = render_pos >= 12;

    if (below || above) {
        /* Calculate the amount of ledger lines to draw and the direction to draw them in. */
        int cur;
        int start = below ? 0 : 12; /* Start position */
        int dir = below ? -2 : 2;
        int end = current_note->value + dir / 2; /* Last line which might have a ledger (if it's even) */

        if (!treble)
            end += dir;

        for (cur = start; below ? cur >= end : cur <= end; cur += dir) {
            gdk_draw_rectangle(
                GDK_DRAWABLE(widget->window),
                gc,
                TRUE,
                STAVE_NOTE_X + STAVE_GAP_Y / 2 - STAVE_LEDGER_LEN / 2, origin_y + STAVE_C_Y - cur * STAVE_SPACING_Y + (STAVE_GAP_Y / 2),
                STAVE_LEDGER_LEN,
                2);
        }
    }

    return TRUE;
}

