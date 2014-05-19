
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Game related stuff
 */
typedef struct {
    int note; // Current note; middle C is 0
    int correct;
    int total;
    GtkWidget *window; // Game window
    GtkWidget *drawing_area; // Image drawing area
    GtkWidget *input; // Text field
    GtkWidget *status;
    GtkWidget *layout; 
    GdkPixbuf *stave; // Stave image
    GdkColor red;
} State;

char *get_note_name(int note) {
    static char *names[] = {"C", "D", "E", "F", "G", "A", "B"};
    return names[abs(note) % 7];
}

int next_note() {
    return -1 + (rand() % 14);
}

void strlower(char *s) {
    for ( ; *s; ++s) *s = tolower(*s);
}

void strupper(char *s) {
    for ( ; *s; ++s) *s = toupper(*s);
}

void submit_answer(State *state, char *answer) {
    static char text[50];
    strupper(answer);

    char *expected = get_note_name(state->note);
    if (strcmp(answer, expected) == 0)
        state->correct++;
    state->total++;

    state->note = next_note();
    gtk_widget_queue_draw(state->drawing_area);

    snprintf(text, sizeof(text), "Note was '%s' | Score: %d of %d", expected, state->correct, state->total);
    gtk_label_set_text(GTK_LABEL(state->status), text);
}

void load_images(GdkPixbuf **stave) {
    GError *err = NULL;
    if (!(*stave = gdk_pixbuf_new_from_file("images/treble.png", &err)))
        printf(err->message);
}

/*
 * Events
 */
gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    /* Redraw the stave */
    State *state = (State *) data;
    GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state(widget)];

    gdk_pixbuf_render_to_drawable(state->stave, GDK_DRAWABLE(widget->window), gc, 0, 0, 0, 0, 250, 200, GDK_RGB_DITHER_MAX, 0, 0);
    
    int y = 162 - (state->note * 12);
    gdk_gc_set_rgb_fg_color(gc, &state->red);
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
            submit_answer((State *) data, input);
        }
        return TRUE;
    }
    return FALSE;
}

/*
 * Main execution point
 */
int main(int argc, char **argv) {
    State state;

    /* Start up GTK */
    gtk_init(&argc, &argv);
    state.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    state.drawing_area = gtk_drawing_area_new();
    state.input = gtk_text_view_new();
    state.layout = gtk_vbox_new(FALSE, 0);
    state.status = gtk_label_new("Name the note on the stave");
    state.correct = state.total = 0;

    load_images(&state.stave);

    gtk_window_set_title(GTK_WINDOW(state.window), "sight-reader");
    gtk_window_set_position(GTK_WINDOW(state.window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(state.window), 250, 240);

    /* Setup our signal handlers */
    g_signal_connect(G_OBJECT(state.drawing_area), "expose_event", G_CALLBACK(expose_event_callback), (gpointer) &state);
    g_signal_connect(G_OBJECT(state.window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(state.window));
    g_signal_connect(G_OBJECT(state.input), "key-press-event", G_CALLBACK(key_press_event_callback), (gpointer) &state);

    gtk_container_add(GTK_CONTAINER(state.window), state.layout);
    gtk_box_pack_start(GTK_BOX(state.layout), state.drawing_area, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(state.layout), state.input, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(state.layout), state.status, FALSE, FALSE, 0);

    srand(time(NULL));
    state.note = next_note();

    gtk_widget_show_all(state.window);
    gtk_main();
    return 0;
}