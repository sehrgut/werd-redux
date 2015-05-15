#include <stdint.h>
#include "fnv1a32.h"

uint32_t fnv1a32(uint32_t d, const uint8_t* buf, long len) {
  //http://svn.apache.org/repos/asf/subversion/trunk/subversion/libsvn_subr/fnv1a.c

  int i;
  for (i=0; i<len; i++) {
    d ^= buf[i];
    d *= FNV1_PRIME_32;
  }

  return d;
}
