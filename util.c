#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include "util.h"


char* strnjoin(char* str[], char* delim, int n) {
  int len = (n-1) * strlen(delim);

  int i;
  for (i=0; i<n; i++)
    len += strlen(str[i]);

  char* out = calloc(len+1, sizeof(char));

  char* p = out;
  for (i=0; i<n; i++) {
    p = stpcpy(p, str[i]);

    if (i<n-1)
      p = stpcpy(p, delim);
  }

  return out;
}

void info(char* fmt, ...) {
#ifdef DEBUG
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
#endif
}
