%{
#include <string.h>
#include "werd_parser.h"
#include "werd_parser.tab.h"

%}

%option prefix="werd_yy"
%option reentrant
%option bison-bridge
%option noyywrap

%%
#.*$                    yylval_param->string=strdup(yytext); return COMMENT;
[A-Z]                   yylval_param->string=strdup(yytext); return CLASS;
[a-z]+                  yylval_param->string=strdup(yytext); return GRAPHEME;
[\t ]*:[\t ]*           return CLASSDELIM;
[\t ]+                  return WHITESPACE;
[\r\n]+                 return EOL;
%%
