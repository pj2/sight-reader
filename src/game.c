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
    game->current_note = 0;

    srand(time(NULL));
    game_next_bar(game);
}

void game_destroy(game *game) {
    main_window_destroy(&game->main_window);
    gtk_main_quit();
}

void game_next_note(game *game, note *note) {
    note->value = rand() % 18 - 3;

    /* 1 in 8 chance of a modifier */
    if ((note->modifiers = rand() % 9) > NOTE_MOD_FLAT)
        note->modifiers = 0; 

    if (game->clef == CLEF_BASS)
        note->value += 2;
}

void game_next_bar(game *game) {
    /* Toggle clef? */
    if (--game->next_clef_swap == 0) {
        game->next_clef_swap = 5 + (rand() % 10);
        game->clef = !game->clef;
    }

    int i;
    for (i = 0; i < NOTES_PER_BAR; i++) {
        game_next_note(game, &game->notes[i]);
    }
}

void game_submit_answer(game *game, note *answer) {
    char label[50];
    char given[NOTE_NAME_MAX_LEN];
    char expected[NOTE_NAME_MAX_LEN];

    note *cur = &game->notes[game->current_note];

    note_get_name(answer, given);
    note_get_name(cur, expected);

    if (note_equals_ignore_pitch(answer, cur))
        game->correct++;
    game->total++;

    /* Advance a note, or create a new set of notes */
    if (++game->current_note >= NOTES_PER_BAR) {
        game_next_bar(game);
        game->current_note = 0;
    }

    cur = &game->notes[game->current_note];
    main_window_set_selected_modifier(&game->main_window, cur->modifiers);
    gtk_widget_queue_draw(game->main_window.drawing_area);

    snprintf(label, sizeof(label), "given=%s, expected=%s | Score: %d of %d", given, expected, game->correct, game->total);
    gtk_label_set_text(GTK_LABEL(game->main_window.status), label);
}

