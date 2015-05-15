#ifndef WERD_PERFECT_HASH_H
#define WERD_PERFECT_HASH_H

struct perfect_hash_s;

typedef const char * (*key_fp)(const void *);
typedef void (*key_destroy_fp)(const char *);

struct perfect_hash_s * phash_create(const void* objects[], int size, key_fp get_key, key_destroy_fp destroy_key);
void phash_destroy(struct perfect_hash_s* hash);

#endif
