#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "classdef.h"
#include "syllable.h"
#include "util.h"

Classdef* classdef_new(char* class) {
  Classdef* s = calloc(1, sizeof(Classdef));
  if (s) {
    s->class = strdup(class);
  }
  return s;
}

void classdef_destroy(Classdef *c) {
  Syllable* cur;
  Syllable* next;

  if (c) {
      next = c->syllables;
      while (next) {
        cur = next;
        next = cur->next;
        syllable_destroy(cur);
      }
      if (c->class) free(c->class);
      free(c);
    }
}

void classdef_add_syllable(Classdef* c, Syllable* s) {
  if (c->syllables_tail) {
    c->syllables_tail->next = s;
    c->syllables_tail = s;
  } else {
    c->syllables = c->syllables_tail = s;
  }
  (c->syllables_n)++;
}

char* classdef_tostring(Classdef* c) {
  char* out;
  char** strings = calloc(c->syllables_n, sizeof(char*));

  Syllable* cur;
  int i;
  for(cur = c->syllables, i=0; cur; cur=cur->next, i++)
    strings[i] = syllable_tostring(cur);

  char* joined = strnjoin(strings, " ", c->syllables_n);

  for(i=0; i<c->syllables_n; i++)
    if (strings[i]) free(strings[i]);

  if (asprintf(&out, "%s:%s", c->class, joined) < 0)
    out = NULL;

  if(joined) free(joined);

  return out;

}
