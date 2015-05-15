#ifndef WERD_SYLLABLE_H
#define WERD_SYLLABLE_H

#include "phoneme.h"

typedef struct syllable_struct Syllable;

struct syllable_struct {
  Phoneme *phonemes;
  Phoneme *phonemes_tail;
  Syllable *next;
  int phonemes_n;
};

Syllable*   syllable_new();
void        syllable_destroy(Syllable *s);

void        syllable_add_phoneme(Syllable* s, Phoneme* p);
void        syllable_append_syllable(Syllable* head, Syllable* tail);
char*       syllable_tostring(Syllable* s);

#endif
