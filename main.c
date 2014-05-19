/* main.c
Author: Joshua Prendergast */

#include "game.h"

int main(int argc, char **argv) {
    State state;

    /* Start up GTK */
    gtk_init(&argc, &argv);
    game_init(&state);

    gtk_widget_show_all(state.window);
    gtk_main();
    return 0;
}