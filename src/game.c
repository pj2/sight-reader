/* game.c
Author: Joshua Prendergast */

#include "game.h"
#include "util.h"
#include "events.h"
#include <string.h>
#include <stdlib.h>

void game_init(game *game) {
    main_window_init(game, &game->main_window);

    game->correct = game->total = 0;
    game->clef = CLEF_TREBLE;
    game->next_clef_swap = 5 + rand() % 15;

    srand(time(NULL));
    game_next_note(game);
}

char *game_get_note_name(int note) {
    static char *names[] = {"C", "D", "E", "F", "G", "A", "B"};
    int index = abs(note) % 7;
    if (note < 0)
        index = 7 - index;

    return names[index];
}

void game_next_note(game *game) {
    game->note = (rand() % 18) - 3;

    /* Toggle clef? */
    if (--game->next_clef_swap == 0) {
        game->next_clef_swap = 5 + (rand() % 10);
        game->clef = !game->clef;
    }

    if (game->clef == CLEF_BASS)
        game->clef += 2;
}

void game_submit_answer(game *game, char *answer) {
    static char text[50];
    strupper(answer);

    char *expected = game_get_note_name(game->note);
    if (strcmp(answer, expected) == 0)
        game->correct++;
    game->total++;

    game_next_note(game);
    gtk_widget_queue_draw(game->main_window.drawing_area);

    snprintf(text, sizeof(text), "Note was '%s' | Score: %d of %d", expected, game->correct, game->total);
    gtk_label_set_text(GTK_LABEL(game->main_window.status), text);
}
