#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "malloc_hook.h"

#include "jemalloc.h"

// hook : malloc, realloc, free, calloc

void *
skynet_malloc(size_t size) {
	return je_malloc(size);
}

void *
skynet_realloc(void *ptr, size_t size) {
	if (ptr == NULL) return skynet_malloc(size);

	return je_realloc(ptr, size);
}

void
skynet_free(void *ptr) {
	if (ptr == NULL) return;
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

