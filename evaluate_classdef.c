#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "classdef.h"
#include "random.h"
#include "util.h"
#include "evaluate_classdef.h"

#define MAXWORDLEN = 255

static void print_known_classes(Classdef** cc, int n) {
  int i;
  for (i=0; i<n; i++)
    printf("%s", cc[i]->class);
  printf("\n");
}

Classdef* find_class_by_name(char* name, Classdef** cc, int n) {
  int i;
  for (i=0; i<n; i++) {
    if (strcmp(name, cc[i]->class) == 0) {
      return cc[i];
    }
  }
  return NULL;
}

char* evaluate_class_by_name(char* name, Classdef** cc, int n) {
  Classdef* c = find_class_by_name(name, cc, n);
  if (c) {
    return evaluate_class(c, cc, n);
  }

  printf("Unable to find class '%s'\n", name);
  return strdup("<UNKNOWN>");
}

static Syllable* pick_syllable(Classdef* c) {
  long r = rand_at_most(c->syllables_n - 1);

  // find r-th syllable
  Syllable* s = c->syllables;
  while (r--)
    s = s->next;

    return s;
}

char* evaluate_class(Classdef* c, Classdef** cc, int n) {

  Syllable* s = pick_syllable(c);
  char** strings = calloc(s->phonemes_n, sizeof(char*));

  int i;
  Phoneme* p = s->phonemes;
  for(i=0; i<s->phonemes_n; i++) {
    if (p->is_class) {
      strings[i] = evaluate_class_by_name(p->text, cc, n);
    } else {
      strings[i] = strdup(p->text);
    }

    p = p->next;
  }

  char* out = strnjoin(strings, "", s->phonemes_n);

  for(i=0; i<s->phonemes_n; i++)
    if(strings[i]) free(strings[i]);

  return out;
}
