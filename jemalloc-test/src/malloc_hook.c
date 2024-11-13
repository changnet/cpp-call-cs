#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "malloc_hook.h"

// turn on MEMORY_CHECK can do more memory check, such as double free
// #define MEMORY_CHECK

#define MEMORY_ALLOCTAG 0x20140605
#define MEMORY_FREETAG 0x0badf00d

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

static void malloc_oom(size_t size) {
	fprintf(stderr, "xmalloc: Out of memory trying to allocate %zu bytes\n",
		size);
	fflush(stderr);
	abort();
}

inline static void*
fill_prefix(char* ptr, size_t sz, uint32_t cookie_size) {
	uint32_t handle = 0xFFFF; //skynet_current_handle();
	struct mem_cookie *p = (struct mem_cookie *)ptr;
	char * ret = ptr + cookie_size;
	p->size = sz;
	p->handle = handle;
#ifdef MEMORY_CHECK
	p->dogtag = MEMORY_ALLOCTAG;
#endif

	memcpy(ret - sizeof(uint32_t), &cookie_size, sizeof(cookie_size));
	return ret;
}

inline static uint32_t
get_cookie_size(char *ptr) {
	uint32_t cookie_size;
	memcpy(&cookie_size, ptr - sizeof(cookie_size), sizeof(cookie_size));
	return cookie_size;
}

inline static void*
clean_prefix(char* ptr) {
	uint32_t cookie_size = get_cookie_size(ptr);
	struct mem_cookie *p = (struct mem_cookie *)(ptr - cookie_size);
	uint32_t handle = p->handle;
#ifdef MEMORY_CHECK
	uint32_t dogtag = p->dogtag;
	if (dogtag == MEMORY_FREETAG) {
		fprintf(stderr, "xmalloc: double free in :%08x\n", handle);
	}
	assert(dogtag == MEMORY_ALLOCTAG);	// memory out of bounds
	p->dogtag = MEMORY_FREETAG;
#endif
	return p;
}


// hook : malloc, realloc, free, calloc

void *
skynet_malloc(size_t size) {
	void* ptr = je_malloc(size + PREFIX_SIZE);
	if(!ptr) malloc_oom(size);
	return fill_prefix(ptr, size, PREFIX_SIZE);
}

void *
skynet_realloc(void *ptr, size_t size) {
	if (ptr == NULL) return skynet_malloc(size);

	uint32_t cookie_size = get_cookie_size(ptr);
	void* rawptr = clean_prefix(ptr);
	void *newptr = je_realloc(rawptr, size+cookie_size);
	if(!newptr) malloc_oom(size);
	return fill_prefix(newptr, size, cookie_size);
}

void
skynet_free(void *ptr) {
	return;
	if (ptr == NULL) return;
	void* rawptr = clean_prefix(ptr);
	je_free(rawptr);
}

void *
skynet_calloc(size_t nmemb, size_t size) {
	uint32_t cookie_n = (PREFIX_SIZE+size-1)/size;
	void* ptr = je_calloc(nmemb + cookie_n, size);
	if(!ptr) malloc_oom(nmemb * size);
	return fill_prefix(ptr, nmemb * size, cookie_n * size);
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
