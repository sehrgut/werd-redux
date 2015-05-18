#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#include "minunit.h"
#include "perfect_hash.h"

int tests_run = 0;


const char* dup_str_as_own_key(const void* obj) {
  return strdup((const char *)obj);
}

const char* get_str_as_own_key(const void* obj) {
  return (const char *)obj;
}

void free_key(const char* key) {
  free((void *)key);
}


const char* alphabet[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
					"L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
					"W", "X", "Y", "Z"};
const int alphabet_n = 26;


static char* test_phash_create() {
	phash_state* func = phash_create((const void**)alphabet, alphabet_n,
									dup_str_as_own_key, free_key);
	phash_destroy(func);

	mu_assert("created null hash", (func != NULL));
	return NULL;
}

static char* test_phash_get() {
	phash_state* func = phash_create((const void**)alphabet, alphabet_n,
									dup_str_as_own_key, free_key);


	int i;
	for(i=0; i<26; i++) {
		const char* good_key = alphabet[i];
		const char* good_obj = (const char *)phash_get(good_key, func);
		
		mu_assert("test_phash_get: lookup failed",
					(good_obj != NULL));
		mu_assert("test_phash_get: bad lookup",
					(!strcmp(good_key, good_obj)));
	}
	
	phash_destroy(func); // Doesn't free if test fails
	
	return NULL;
}

static char* test_phash_get_validated() {
	phash_state* func = phash_create((const void**)alphabet, alphabet_n,
									dup_str_as_own_key, free_key);

	const char* good_key = alphabet[0];
	const char* good_obj = (const char*)phash_get_validated(good_key, func, dup_str_as_own_key, free_key);
	
	const char* bad_key = "asdfasdfasdf";
	const char* bad_obj = (const char *)phash_get_validated(bad_key, func, dup_str_as_own_key, free_key);
	phash_destroy(func);
	
	mu_assert("test_phash_get_validated: bad lookup succeeded, should've failed",
				(bad_obj == NULL));

	mu_assert("test_phash_get_validated: good lookup failed",
				(good_obj != NULL));
	
	mu_assert("test_phash_get: bad lookup of good key",
				(!strcmp(good_key, good_obj)));
	return NULL;
}

static char* all_tests() {
	mu_run_test(test_phash_create);
	mu_run_test(test_phash_get);
	mu_run_test(test_phash_get_validated);
	return NULL;
}










int main() {
	char *result = all_tests();
	if(result)
		printf("[Failure] %s\n", result);
	else
		printf("All tests passed!\n");
	
	printf("Tests run: %d\n", tests_run);
	return (result != NULL);
}