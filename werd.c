#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "classdef.h"
#include "statement.h"
#include "evaluate_classdef.h"
#include "util.h"

void werd_srand() {
  struct timeval time;
  gettimeofday(&time,NULL);
  info("[werd_srand()] Time: %d.%d\n", time.tv_sec, time.tv_usec);
  long long seed = (time.tv_sec * 1000) + (time.tv_usec / 1000);
  info("[werd_srand()] Seed: %lld\n", seed);
  srandom(seed);
}

static int count_classes_in_stmt_list(Statement* s) {
  int n = 0;

  while (s) {
    if (s->tag == CLASSDEF_TAG) {
      n++;
    }
    s = s->next;
  }

  return n;
}

static Classdef** find_classes_from_stmt_list(Statement* s, int* cn) {
  int n = count_classes_in_stmt_list(s);
  Classdef** cc = calloc(n, sizeof(Classdef*));

  int i = 0;
  while (i < n) {
    if (s->tag == CLASSDEF_TAG) {
      cc[i] = s->classdef;
      i++;
    }
    s = s->next;
  }

  *cn = n;
  return cc;
}

void werd_print(Statement* s, int n) {
  int cn;

  Classdef** cc = find_classes_from_stmt_list(s, &cn);
  if (cc) {
    Classdef* root = find_class_by_name("W", cc, cn);

    int i;
    for(i=0; i<n; i++) {
      char* word = evaluate_class(root, cc, cn);
      printf("Word: '%s'\n", word);
      free(word);
    }

    free(cc);
  } else {
    fprintf(stderr, "Could not find classes in statement list %p.\n", s);
  }
}

static void dump_statements(Statement* s) {
  Statement* cur = s;
  while (cur) {
    char* str = stmt_tostring(cur);
    printf("%s\n", (str?str:""));
    if(str) free(str);
    cur = cur->next;
  }
}

static Statement* parse_stdin() {
  Statement* output = NULL;

  //todo: store myscanner in context struct?
  // http://plindenbaum.blogspot.com/2009/12/parsing-genetic-code-using-flex-and_14.html
  void* myscanner;
  werd_yylex_init(&myscanner);
  werd_yyparse(myscanner, &output);
  werd_yylex_destroy(myscanner);

  return output;
}

void print_usage() {
  printf("Usage: werd [-v] [-n <count>]\n");
}

int main(int argc, char* argv[])
{

  int verbosity = 0;
  int num_words = 10;

  static struct option lopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"usage", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    {"number", required_argument, NULL, 'n'},
    {NULL, 0, NULL, 0}
  };

  static char* shopts = "hvn:";
  int opt = 0;
  int long_index = 0;
  while ((opt = getopt_long(argc, argv, shopts, lopts, &long_index )) != -1) {
    switch (opt) {
      case 'h':
        print_usage();
        exit(EXIT_SUCCESS);
      case 'v':
        verbosity = 1;
        break;
      case 'n':
        errno = 0;
        int tmp_n = strtol(optarg, NULL, 10);
        if (errno) {
          fprintf(stderr, "Invalid argument: '%s'\n", optarg);
          exit(EXIT_FAILURE);
        }
        num_words = tmp_n;
        break;
      default:
        print_usage();
        exit(EXIT_FAILURE);
    }
  }

  werd_srand();

  Statement* statements = parse_stdin();

  if (verbosity > 0)
    dump_statements(statements);

  werd_print(statements, num_words);

  exit(EXIT_SUCCESS);
  return 0;
}
