#include <malloc.h>
#include <stdio.h>
#include "syllable.h"
#include "phoneme.h"
#include "util.h"

Syllable* syllable_new() {
  Syllable* s = calloc(1, sizeof(Syllable));
  return s;
}

void syllable_destroy(Syllable *s) {
  Phoneme* cur;
  Phoneme* next;

  if (s) {
      next = s->phonemes;
      while (next) {
        cur = next;
        next = cur->next;
        phoneme_destroy(cur);
      }
    free(s);
  }
}

void syllable_add_phoneme(Syllable* s, Phoneme* p) {
  if (s->phonemes_tail) {
    s->phonemes_tail->next = p;
    s->phonemes_tail = p;
  } else {
    s->phonemes = s->phonemes_tail = p;
  }
  (s->phonemes_n)++;
}

char* syllable_tostring(Syllable* s) {
  char** strings = calloc(s->phonemes_n, sizeof(char*));

  Phoneme* cur;
  int i;
  for (cur=s->phonemes, i=0; cur; cur=cur->next, i++)
    strings[i] = phoneme_tostring(cur);

  char* joined = strnjoin(strings, "", s->phonemes_n);

  for (i=0; i<s->phonemes_n; i++)
    if(strings[i]) free(strings[i]);

  return joined;
}

void syllable_append_syllable(Syllable* head, Syllable* tail) {
  Syllable* cur;
  do {
    cur = head->next;
  } while (cur->next);

  cur->next = tail;
}
