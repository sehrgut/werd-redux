#include <stdlib.h> // For random(), RAND_MAX

// http://stackoverflow.com/a/6852396
// Assumes 0 <= max <= RAND_MAX
// Returns in the half-open interval [0, max]
long rand_at_most(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}

long rand_in_range(long min, long max) {
  long domain = max - min;
  return rand_at_most(domain) + min;
}
