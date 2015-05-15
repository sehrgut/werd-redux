#ifndef WERD_CLASSDEF_H
#define WERD_CLASSDEF_H

#include "syllable.h"

typedef struct classdef_struct Classdef;

struct classdef_struct {
  char*     class;
  Syllable* syllables;
  Syllable* syllables_tail;
  int       syllables_n;
};

Classdef*   classdef_new(char* class);
void        classdef_destroy(Classdef *c);

void        classdef_add_syllable(Classdef* c, Syllable* s);
char*       classdef_tostring(Classdef* c);

#endif
