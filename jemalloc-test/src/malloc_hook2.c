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

#define PREFIX_SIZE 32//sizeof(struct mem_cookie)

#include "jemalloc.h"

// hook : malloc, realloc, free, calloc

void *
skynet_malloc(size_t size) {
	return je_malloc(size);;
}

void *
skynet_realloc(void *ptr, size_t size) {
	if (ptr == NULL) return skynet_malloc(size);
	return je_realloc(ptr, size);
}

void
skynet_free(void *ptr) {
	if (!ptr) return;
	je_free(ptr);
}

void *
skynet_calloc(size_t nmemb, size_t size) {
	return je_calloc(nmemb, size);
}

void *
skynet_memalign(size_t alignment, size_t size) {
	return je_memalign(alignment, size);
}

void *
skynet_aligned_alloc(size_t alignment, size_t size) {
	return je_aligned_alloc(alignment, size);
}

int
skynet_posix_memalign(void **memptr, size_t alignment, size_t size) {
	return je_posix_memalign(memptr, alignment, size);
}
