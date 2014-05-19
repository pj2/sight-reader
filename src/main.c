/* main.c
Author: Joshua Prendergast */

#include "game.h"

int main(int argc, char **argv) {
    game game;

    /* Start up GTK */
    gtk_init(&argc, &argv);
    game_init(&game);

    gtk_widget_show_all(game.main_window.window);
    gtk_main();
    return 0;
}
