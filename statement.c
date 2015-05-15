#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "statement.h"
#include "classdef.h"

static Statement* stmt_new();

Statement* stmt_new_list() {
  Statement* s = stmt_new();
  if(s) {
    s->tag = LIST_HEAD_TAG;
    s->tail = s;
  }

  return s;
}

Statement* stmt_new_comment(char* c) {
  Statement* s = stmt_new();

  if(s) {
    s->tag = COMMENT_TAG;
    s->comment = strdup(c);
  }

  return s;
}

Statement* stmt_new_classdef(Classdef* c) {
  Statement* s = stmt_new();

  if (s) {
    s->tag = CLASSDEF_TAG;
    s->classdef = c;
  }

  return s;
}

Classdef* stmt_move_classdef(Statement* s) {
  Classdef* out = NULL;
  if (s && s->tag == CLASSDEF_TAG) {
    out = s->classdef;
    s->classdef = NULL;
  }

  return out;
}

void stmt_destroy(Statement* s) {
  if (s) {
    switch(s->tag) {
      case COMMENT_TAG:
        if (s->comment) free(s->comment);
        break;
      case CLASSDEF_TAG:
        if (s->classdef) free(s->classdef);
        break;
      default:
        break;
    }

    free(s);
  }
}

static Statement* stmt_new() {
  Statement* s = calloc(1, sizeof(Statement));
  return s;
}

char* stmt_tostring(Statement* s) {
  switch(s->tag) {
    case COMMENT_TAG:
      return strdup(s->comment);
    case CLASSDEF_TAG:
      return classdef_tostring(s->classdef);
    case LIST_HEAD_TAG:
      return strdup("<HEAD>");
  }
}

static void stmt_append_any(Statement* head, Statement* tail) {
  Statement* cur;
  do {
    cur = head->next;
  } while (cur->next);

  cur->next = tail;
}

bool stmt_append(Statement* head, Statement* tail) {
  if (head->tag == LIST_HEAD_TAG) {
    head->tail->next = tail;
    head->tail = tail;
    return true;
  }
  return false;
}
