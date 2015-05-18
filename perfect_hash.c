#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "perfect_hash.h"
#include "fnv1a32.h"

/***** Types *****/

struct phash_state_s {
  int             len;
  uint32_t        seed;
  uint32_t*       G;
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

/***** Local Function Declarations *****/
static void phash_dump_tables(const int* G, const void** V, int size);
static void phash_dump(struct phash_state_s* hashfunc);

/***** Local Functions *****/

static uint32_t hash_index(uint32_t d, const char* key, int size) {
  // http://stevehanov.ca/blog/index.php?id=119
  if (!d) d = FNV1_PRIME_32;
  return fnv1a32(d, (uint8_t*)key, strlen(key)) % size;
}

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

/***** Debug Functions *****/
void phash_dump_tables(const int* G, const void** V, int size) {
  int i;
  for(i=0; i<size; i++)
    printf("\tG[%02d] = %d\tV[%02d] = %p\n",
      i, G[i], i, V[i]);
}

void phash_dump(struct phash_state_s* hashfunc) {
  printf("struct phash_state_s %p\n", hashfunc);
  printf("\tsize: %d\n", hashfunc->len);
  printf("\tseed: %d\n", hashfunc->seed);
  phash_dump_tables(hashfunc->G, hashfunc->V, hashfunc->len);
}

/***** External Functions *****/

void phash_destroy(struct phash_state_s* hash) {
  if (hash) {
    if (hash->G) free(hash->G);
    if (hash->V) free(hash->V);
    free(hash);
  }
}

// todo: "digest algo" enum and parameter
struct phash_state_s * phash_create(const void* objects[], int size, key_fp get_key, key_destroy_fp destroy_key) {
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

      dvals[b.id] = dd;

      free(kk);

  }


  free(hashes);
  free(buckets);
  free(items);

  if (destroy_key)
    for(i=0; i<size; i++)
      destroy_key(keys[i]), keys[i] = NULL;

  free(keys);

  // todo: this should be the canonical store of these values all throughout
  struct phash_state_s* out = calloc(1, sizeof(struct phash_state_s));

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

const void* phash_get(const char* key, const struct phash_state_s* hashfunc) {
  int g = hash_index(hashfunc->seed, key, hashfunc->len);
  int d = hashfunc->G[g];
  int v = hash_index(d, key, hashfunc->len);
  return hashfunc->V[v];
}

const void* phash_get_validated(const char* key,
								const struct phash_state_s* hashfunc,
								key_fp get_key, key_destroy_fp destroy_key) {
		
  const void* obj = phash_get(key, hashfunc);
  const char* obj_key = get_key(obj);

  if (strcmp(key, obj_key) != 0)
    obj = NULL;

  if (destroy_key)
    destroy_key(obj_key);

  return obj;
}

/***** Test harness *****/
/*
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

  struct phash_state_s* func = phash_create((const void**)alphabet, 26, dup_str_as_own_key, free_key);
  phash_dump(func);

  int i;
  for(i=0; i<26; i++) {
    char* good_key = alphabet[i];
    char* good_obj = (char *)phash_get(good_key, func);
    char* good_obj_validated = (char *)phash_get_validated(good_key, func, dup_str_as_own_key, free_key);
    printf("match: %s->%s (%s)\n", good_key, good_obj, good_obj_validated);
  }


  exit(EXIT_SUCCESS);
}
*/