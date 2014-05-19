/* util.c
Author: Joshua Prendergast */

#include "util.h"
#include <ctype.h>

void strlower(char *s) {
    for ( ; *s; ++s) *s = tolower(*s);
}

void strupper(char *s) {
    for ( ; *s; ++s) *s = toupper(*s);
}