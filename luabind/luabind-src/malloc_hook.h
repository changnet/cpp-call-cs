#ifndef SKYNET_MALLOC_HOOK_H
#define SKYNET_MALLOC_HOOK_H

#include <stdlib.h>
#include <stdbool.h>
#include <lua.h>

#include <stddef.h>

#define skynet_malloc malloc
#define skynet_calloc calloc
#define skynet_realloc realloc
#define skynet_free free
#define skynet_memalign memalign
#define skynet_aligned_alloc aligned_alloc
#define skynet_posix_memalign posix_memalign

void * skynet_malloc(size_t sz);
void * skynet_calloc(size_t nmemb,size_t size);
void * skynet_realloc(void *ptr, size_t size);
void skynet_free(void *ptr);
char * skynet_strdup(const char *str);
void * skynet_lalloc(void *ptr, size_t osize, size_t nsize);	// use for lua
void * skynet_memalign(size_t alignment, size_t size);
void * skynet_aligned_alloc(size_t alignment, size_t size);
int skynet_posix_memalign(void **memptr, size_t alignment, size_t size);

extern size_t malloc_used_memory(void);
extern size_t malloc_memory_block(void);
extern void   memory_info_dump(const char *opts);
extern size_t mallctl_int64(const char* name, size_t* newval);
extern int    mallctl_opt(const char* name, int* newval);
extern bool   mallctl_bool(const char* name, bool* newval);
extern int    mallctl_cmd(const char* name);
extern void   dump_c_mem(void);
extern int    dump_mem_lua(lua_State *L);
extern size_t malloc_current_memory(void);

#endif /* SKYNET_MALLOC_HOOK_H */

