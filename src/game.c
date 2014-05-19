/* game.c
Author: Joshua Prendergast */

#include "game.h"
#include "util.h"
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

void game_next_note(game *game) {
    game->note.value = rand() % 18 - 3;

    /* 1 in 10 chance of a modifier */
    if ((game->note.modifiers = rand() % 11) > NOTE_MOD_FLAT)
        game->note.modifiers = 0; 

    /* Toggle clef? */
    if (--game->next_clef_swap == 0) {
        game->next_clef_swap = 5 + (rand() % 10);
        game->clef = !game->clef;
    }

    if (game->clef == CLEF_BASS)
        game->note.value += 2;
}

void game_submit_answer(game *game, note *answer) {
    char label[50];
    char given[NOTE_NAME_MAX_LEN];
    char expected[NOTE_NAME_MAX_LEN];

    note_get_name(answer, given);
    note_get_name(&game->note, expected);

    if (note_equals_ignore_pitch(answer, &game->note))
        game->correct++;
    game->total++;

    game_next_note(game);
    gtk_widget_queue_draw(game->main_window.drawing_area);

    snprintf(label, sizeof(label), "given=%s, expected=%s | Score: %d of %d", given, expected, game->correct, game->total);
    gtk_label_set_text(GTK_LABEL(game->main_window.status), label);
}

