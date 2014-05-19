/* note.h
Author: Joshua Prendergast */

#ifndef NOTE_H
#define NOTE_H

#define CLEF_TREBLE 0
#define CLEF_BASS 1

#define NOTE_MOD_NONE 0
#define NOTE_MOD_SHARP 1
#define NOTE_MOD_FLAT 2

#define NOTE_NAME_MAX_LEN 3
#define NOTE_MAX 7

typedef struct {
    int value; // Current note; middle C is 0
    int modifiers; // Note modifier (e.g. sharp, flat)
} note;

int note_equals_ignore_pitch(note *a, note *b);

void note_get_name(note *note, char *name);

int note_without_pitch(int value);

#endif // NOTE_H
