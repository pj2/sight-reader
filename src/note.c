/* note.c
Author: Joshua Prendergast */

#include "note.h"
#include <stdlib.h>

int note_equals_ignore_pitch(note *a, note *b) {
    /* Return 1 if equal (pitch is ignored) */
    return note_without_pitch(a->value) == note_without_pitch(b->value) && a->modifiers == b->modifiers;
}

void note_get_name(note *note, char *name) {
    static char names[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};

    if (note->modifiers == NOTE_MOD_NONE)
        name[1] = '\0';
    else if ((note->modifiers & NOTE_MOD_FLAT) == NOTE_MOD_FLAT)
        name[1] = 'b';
    else if ((note->modifiers & NOTE_MOD_SHARP) == NOTE_MOD_SHARP)
        name[1] = '#';

    name[0] = names[note_without_pitch(note->value)];
    name[2] = '\0';
}

int note_without_pitch(int value) {
    int out = abs(value) % NOTE_MAX;
    if (value < 0)
        out = NOTE_MAX - out;
    return out;
}
