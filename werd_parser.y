%{
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include "werd_parser.h"
#include "classdef.h"
#include "werd.h"
#include "util.h"
%}

%define api.prefix {werd_yy}
%define api.pure full
%lex-param {void* scanner}
%parse-param {void* scanner}{Statement** output_p}

%union {
  char*       string;
  Classdef*   cd;
  Syllable*   syl;
  Phoneme*    ph;
  Statement*  stmt;
}

%token <string>COMMENT <string>CLASS CLASSDELIM WHITESPACE <string>GRAPHEME EOL
%type <ph>phoneme
%type <syl>syllable
%type <cd>syllables
%type <cd>classdef
%type <stmt>statement
%type <stmt>statements

%{
#include "werd_parser.yy.h"
%}

%%

statements : %empty { *output_p = $$ = stmt_new_list(); }
           | statements statement EOL { //Bug: doesn't recognize the last statement in the file
             stmt_append($$, $2);
           }
           ;

statement : COMMENT {
              $$ = stmt_new_comment($1);
              info("Statement type: comment.\n");
            }
          | classdef {
              $$ = stmt_new_classdef($1);
              info("Statement type: classdef (%p).\n", $1);
            }
          ;

classdef : CLASS CLASSDELIM syllables {
          $3->class = strdup($1);
          char* s = classdef_tostring($3);
          info("Recognized classdef (%p): '%s'.\n", $3, (s?s:""));
          if(s) free(s);
          $$ = $3;
          }
         ;

syllables : syllable {
            $$ = classdef_new("");
            classdef_add_syllable($$, $1);
            char* s = syllable_tostring($1);
            info("First syllable of classdef: '%s'.\n", (s?s:""));
            if(s) free(s);
          }
          | syllables WHITESPACE syllable {
            classdef_add_syllable($1, $3);
            char* s = syllable_tostring($3);
            info("Another syllable: '%s'.\n", (s?s:""));
            if(s) free(s);
          }
          ;

syllable : %empty { $$ = syllable_new(); }
         | syllable phoneme {
            syllable_add_phoneme($1, $2);
            char* s = phoneme_tostring($2);
            info("Found phoneme '%s'.\n", (s?s:""));
            if(s) free(s);
            //$$ = $1; //inherits old $$ value
          }
         ;

phoneme : GRAPHEME {$$ = phoneme_new($1, false);}
        | CLASS {$$ = phoneme_new($1, true);}
        ;

%%

void werd_yyerror(yyscan_t scanner, Statement** output, const char *str)
//void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
/*
int main()
{

      Statement* output = NULL;
      void* myscanner;
      werd_yylex_init(&myscanner);
      werd_yyparse(myscanner, &output);
      werd_yylex_destroy(myscanner);


        Statement* cur = output;
        while (cur) {
          char* s = stmt_tostring(cur);
          printf("%s\n", (s?s:""));
          if(s) free(s);
          cur = cur->next;
        }

        printf("Output @ %p\n", output);
        werd_srand();
        werd_print(output, 10);

        return 0;
}
*/
