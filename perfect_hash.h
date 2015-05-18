#ifndef WERD_PERFECT_HASH_H
#define WERD_PERFECT_HASH_H

typedef struct phash_state_s phash_state;

typedef const char * (*key_fp)(const void *);
typedef void (*key_destroy_fp)(const char *);

phash_state* phash_create(const void* objects[], int size,
							key_fp get_key, key_destroy_fp destroy_key);
void phash_destroy(phash_state* hash);

const void* phash_get(const char* key, const phash_state* hashfunc);
const void* phash_get_validated(const char* key, const phash_state* hashfunc,
								key_fp get_key, key_destroy_fp destroy_key);

#endif
