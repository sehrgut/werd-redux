#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define FNV1_PRIME_32 0x01000193

static uint32_t fnv1a32(uint32_t d, char* str) {
  // http://stevehanov.ca/blog/index.php?id=119
  //http://svn.apache.org/repos/asf/subversion/trunk/subversion/libsvn_subr/fnv1a.c

  int i, n;
  for (i=0, n=strlen(str); i<n; i++) {
    d ^= str[i];
    d *= FNV1_PRIME_32;
  }

  return d;
}

static uint32_t hash_index(uint32_t d, char* key, int size) {
  if (!d) d = FNV1_PRIME_32;
  return fnv1a32(d, key) % size;
}

struct perfect_hash_s {
  char**    keys;
  int       len;
  uint32_t* G;
  void**    V;
};

typedef struct bucket_s {
  int   size;
  int   id;
} Bucket;

static bool test_dval(char* keys[], int n, uint32_t d, bool used[], int size) {
  bool* seen = calloc(size, sizeof(bool));

  bool ok = true;
  int i;
  for (i=0; i<n && ok; i++) {
    uint32_t hash = hash_index(d, keys[i], size);
    ok = !(used[hash] || seen[hash]);
    seen[hash] = true;
  }

  free(seen);

  return ok;
}

static int cmp_bucket_size(const void* a, const void* b) {
  return ((Bucket*)a)->size - ((Bucket*)b)->size;
}

static int cmp_bucket_size_desc(const void* a, const void* b) {
  return -(cmp_bucket_size(a, b));
}

struct perfect_hash_s create_hash(char* keys[], int size) {
  uint32_t* hashes = calloc(size, sizeof(uint32_t));
  uint32_t* dvals = calloc(size, sizeof(uint32_t));
  Bucket* buckets = calloc(size, sizeof(Bucket));
  bool* used = calloc(size, sizeof(bool));

  int i;

  // initialize buckets
  for (i=0; i<size; i++) {
    buckets[i].id = i;
  }

  // hash keys and count bucket sizes
  for (i=0; i<size; i++) {
    uint32_t hash = hash_index(0, keys[i], size);
    hashes[i] = hash;
    buckets[hash].size++;
  }

  // sort buckets in descending order by size
  qsort(buckets, size, sizeof(Bucket), cmp_bucket_size_desc);

  int bucketpos;
  for (bucketpos=0; bucketpos<size; bucketpos++) {
      Bucket b = buckets[bucketpos];

      printf("Processing bucket %d (id=%d, size=%d)\n", bucketpos, b.id, b.size);
      int kn = b.size;

      if (!kn) break;

      char** kk = calloc(kn, sizeof(char*));

      int keypos, kkpos;
      for (keypos=0, kkpos=0; keypos<size && kkpos<kn; keypos++) {
        if (hashes[keypos] == b.id) {
          kk[kkpos++] = keys[keypos];
        }
      }

      // find workable dval
      int dd = 1;
      while (test_dval(kk, kn, dd, used, size) == false) {
        dd++;
      }

      dvals[bucketpos] = dd;

      // note used positions with found dval
      for (kkpos=0; kkpos<kn; kkpos++) {
        uint32_t hash = hash_index(dd, kk[kkpos], size);
        used[hash] = true;
      }

      free(kk);

  }


  for (i=0; i<size; i++) {
    //printf("Bucket %d: %d members\n", buckets[i].id, buckets[i].size);
    printf("Dval %d: %d\n", i, dvals[i]);
    //printf("Used %d: %s\n", i, used[i] ? "true" : "false");
  }

  free(hashes);
  free(buckets);
  free(dvals);
  free(used);

  return (struct perfect_hash_s){NULL, 0, NULL, NULL};
}

typedef const char * (*key_fp)(const void *);

void * phash_create(const void * contents, key_fp get_key) {

}

void main() {
  char* alphabet[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

  create_hash(alphabet, 26);
}
