#ifndef WERD_STATEMENT_H
#define WERD_STATEMENT_H

#include <stdbool.h>
#include "classdef.h"

enum stmt_tag { LIST_HEAD_TAG, COMMENT_TAG, CLASSDEF_TAG};

typedef struct stmt_s Statement;

struct stmt_s {
  enum stmt_tag tag;
  union {
    char* comment;
    Classdef* classdef;
    Statement* tail;
  };
  Statement* next;
};

Statement* stmt_new_comment(char* c);
Statement* stmt_new_classdef(Classdef* c);
Statement* stmt_new_list();
Classdef* stmt_move_classdef(Statement* s);
void stmt_destroy(Statement* s);
bool stmt_append(Statement* head, Statement* tail);
char* stmt_tostring(Statement* s);

#endif
