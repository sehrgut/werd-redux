#ifndef WERD_PHONEME_H
#define WERD_PHONEME_H

#include <stdbool.h>

typedef struct phoneme_struct Phoneme;

struct phoneme_struct {
  char *text;
  bool is_class;
  Phoneme* next;
};

Phoneme*  phoneme_new(char *text, bool is_class);
void      phoneme_destroy(Phoneme *p);
char*     phoneme_tostring(Phoneme *p);
#endif
