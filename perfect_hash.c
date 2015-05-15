#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "perfect_hash.h"
#include "fnv1a32.h"

static uint32_t hash_index(uint32_t d, const char* key, int size) {
  // http://stevehanov.ca/blog/index.php?id=119
  if (!d) d = FNV1_PRIME_32;
  return fnv1a32(d, (uint8_t*)key, strlen(key)) % size;
}

struct perfect_hash_s {
  int       len;
  uint32_t  seed;
  uint32_t* G;
  const void**    V;
};

typedef struct bucket_s {
  int   size;
  int   id;
} Bucket;

typedef struct item_s {
  const void* object;
  const char* key;
} Item;

static bool test_dval(const Item* items[], int n, uint32_t d, int size, const void* V[]) {
  static bool* seen = NULL;
  static int seen_size = 0;

  if (!seen || seen_size < size) {
    free(seen);
    seen = calloc(size, sizeof(bool));
    seen_size = size;
  } else {
    memset(seen, 0x00, sizeof(bool) * size);
  }

  static uint32_t* hashes = NULL;
  static int hashes_size = 0;

  if (!hashes || hashes_size < n) {
    free(hashes);
    hashes = calloc(n, sizeof(uint32_t));
    hashes_size = n;
  } else {
    memset(hashes, 0x00, sizeof(uint32_t) * n);
  }

  bool ok = true;
  int i;
  for (i=0; i<n && ok; i++) {
    uint32_t hash = hashes[i] = hash_index(d, items[i]->key, size);
    ok = !(V[hash] || seen[hash]);
    seen[hash] = true;
  }

  if (ok)
    for (i=0; i<n; i++)
      V[hashes[i]] = items[i]->object;

  return ok;
}

static int cmp_bucket_size(const void* a, const void* b) {
  return ((Bucket*)a)->size - ((Bucket*)b)->size;
}

static int cmp_bucket_size_desc(const void* a, const void* b) {
  return -(cmp_bucket_size(a, b));
}

void phash_destroy(struct perfect_hash_s* hash) {
  if (hash) {
    if (hash->G) free(hash->G);
    if (hash->V) free(hash->V);
    free(hash);
  }
}

// todo: "digest algo" enum and parameter
struct perfect_hash_s * phash_create(const void* objects[], int size, key_fp get_key, key_destroy_fp destroy_key) {
  uint32_t* hashes = calloc(size, sizeof(uint32_t)); // todo: move hashes into item_s
  uint32_t* dvals = calloc(size, sizeof(uint32_t));
  Bucket* buckets = calloc(size, sizeof(Bucket));
  const char** keys = calloc(size, sizeof(char*)); // todo: move keys into item_s
  Item* items = calloc(size, sizeof(Item));

  int i;

  // initialize buckets
  for (i=0; i<size; i++) {
    buckets[i].id = i;
  }

  // hash keys and count bucket sizes
  for (i=0; i<size; i++) {
    keys[i] = get_key(objects[i]);
    items[i] = (Item){objects[i], keys[i]};
    uint32_t hash = hash_index(0, keys[i], size);
    hashes[i] = hash;
    buckets[hash].size++;
  }

  // sort buckets in descending order by size
  qsort(buckets, size, sizeof(Bucket), cmp_bucket_size_desc);

  int bucketpos;
  const void** V = calloc(size, sizeof(void*));
  for (bucketpos=0; bucketpos<size; bucketpos++) {
      Bucket b = buckets[bucketpos];

      printf("Processing bucket %d (id=%d, size=%d)\n", bucketpos, b.id, b.size);
      int kn = b.size;

      if (!kn) break;

      const Item** kk = calloc(kn, sizeof(Item*));

      int keypos, kkpos;
      for (keypos=0, kkpos=0; keypos<size && kkpos<kn; keypos++) {
        if (hashes[keypos] == b.id) {
          kk[kkpos++] = &items[keypos];
        }
      }

      // find workable dval
      int dd = 1;
      while (test_dval(kk, kn, dd, size, V) == false) {
        dd++;
      }

      dvals[bucketpos] = dd;

      free(kk);

  }


  for (i=0; i<size; i++) {
    //printf("Bucket %d: %d members\n", buckets[i].id, buckets[i].size);
    printf("Dval %d: %d\n", i, dvals[i]);
  }

  free(hashes);
  free(buckets);
  free(items);

  if (destroy_key)
    for(i=0; i<size; i++)
      destroy_key(keys[i]), keys[i] = NULL;

  free(keys);

  // todo: this should be the canonical store of these values all throughout
  struct perfect_hash_s* out = calloc(1, sizeof(struct perfect_hash_s));

  if (out){
    out->len = size;
    out->seed = 0;
    out->G = dvals;
    out->V = V;
  } else {
    if (dvals) free(dvals);
    if (V) free(V);
  }

  return out;
}


/***** Test harness *****/

const char* dup_str_as_own_key(const void* obj) {
  return strdup((const char *)obj);
}

const char* get_str_as_own_key(const void* obj) {
  return (const char *)obj;
}

void free_key(const char* key) {
  free((void *)key);
}

void main() {
  char* alphabet[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

  phash_create((const void**)alphabet, 26, dup_str_as_own_key, free_key);

  //phash_create((const void**)alphabet, 26, get_str_as_own_key);

  exit(EXIT_SUCCESS);
}
