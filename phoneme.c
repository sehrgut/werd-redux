#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include "phoneme.h"

Phoneme* phoneme_new(char *text, bool is_class) {
  Phoneme* p = calloc(1, sizeof(Phoneme));
  if (p) {
    p->text = strdup(text);
    p->is_class = is_class;
  }
  return p;
}

void phoneme_destroy(Phoneme *p) {
  if (p) {
    if (p->text) free(p->text);
    free(p);
  }
}

char* phoneme_tostring(Phoneme *p) {
  char* out;
  char* fmt = p->is_class ? "<%s>" : "%s";
  if (asprintf(&out, fmt, p->text) < 0)
    return NULL;
  return out;
}
