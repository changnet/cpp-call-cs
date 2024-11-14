#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "malloc_hook.h"

// turn on MEMORY_CHECK can do more memory check, such as double free
#define MEMORY_CHECK

struct mem_cookie {
	size_t size;
	uint32_t handle;
#ifdef MEMORY_CHECK
	uint32_t dogtag;
#endif
	uint32_t cookie_size;	// should be the last
};

#define PREFIX_SIZE sizeof(struct mem_cookie)

#include "jemalloc.h"

// hook : malloc, realloc, free, calloc

void *
skynet_malloc(size_t size) {
	void* ptr = je_malloc(size + PREFIX_SIZE);
	void *new_ptr = (char *)ptr + PREFIX_SIZE;
	
	memset(new_ptr, 0, size); // check memory is ok
	return new_ptr;
}

void *
skynet_realloc(void *ptr, size_t size) {
	if (ptr == NULL) return skynet_malloc(size);
	void *new_ptr = je_realloc((char *)ptr - PREFIX_SIZE, size + PREFIX_SIZE);

	return (char *)new_ptr + PREFIX_SIZE;
}

void
skynet_free(void *ptr) {
	return; // no need for this test
}

void *
skynet_calloc(size_t nmemb, size_t size) {
	uint32_t cookie_n = (PREFIX_SIZE+size-1)/size;
	void* ptr = je_calloc(nmemb + cookie_n, size);

	assert(cookie_n * size >= PREFIX_SIZE);
	return (char *)ptr + PREFIX_SIZE;
}

void *
skynet_memalign(size_t alignment, size_t size) {
	assert(false); // no need for this test

	return NULL;
}

void *
skynet_aligned_alloc(size_t alignment, size_t size) {
	assert(false); // no need for this test

	return NULL;
}

int
skynet_posix_memalign(void **memptr, size_t alignment, size_t size) {
	assert(false); // no need for this test

	return 0;
}
