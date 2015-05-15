#ifndef WERD_WERD_PARSER_H
#define WERD_WERD_PARSER_H

#include "syllable.h"
#include "classdef.h"
#include "phoneme.h"
#include "statement.h"

// Shouldn't need this, but ''%option prefix' doesnt't
// seem to set the name of YYSTYPE
#define YYSTYPE WERD_YYSTYPE

#define YY_DECL int werd_yylex \
	(YYSTYPE* yylval_param, void* yyscanner, Statement** output)

void yyerror(void* scanner, Statement** output, const char *str);



#endif
