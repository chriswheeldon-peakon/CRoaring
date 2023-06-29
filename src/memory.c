#include <roaring/memory.h>
#include <stdlib.h>

// without the following, we get lots of warnings about posix_memalign
#ifndef __cplusplus
extern int posix_memalign(void **__memptr, size_t __alignment, size_t __size);
#endif  //__cplusplus // C++ does not have a well defined signature

// portable version of  posix_memalign
static void *roaring_bitmap_aligned_malloc(size_t alignment, size_t size) {
    void *p;
#ifdef _MSC_VER
    p = _aligned_malloc(size, alignment);
#elif defined(__MINGW32__) || defined(__MINGW64__)
    p = __mingw_aligned_malloc(size, alignment);
#else
    // somehow, if this is used before including "x86intrin.h", it creates an
    // implicit defined warning.
    if (posix_memalign(&p, alignment, size) != 0) return NULL;
#endif
    return p;
}

static void roaring_bitmap_aligned_free(void *memblock) {
#ifdef _MSC_VER
    _aligned_free(memblock);
#elif defined(__MINGW32__) || defined(__MINGW64__)
    __mingw_aligned_free(memblock);
#else
    free(memblock);
#endif
}

static void *default_malloc(size_t size, void *context) {
    return malloc(size);
}

static void *default_realloc(void *ptr, size_t size, void *context) {
    return realloc(ptr, size);
}

static void *default_calloc(size_t count, size_t size, void *context) {
    return calloc(count, size);
}

static void default_free(void *ptr, void *context) {
    free(ptr);
}

static void *default_aligned_malloc(size_t alignment, size_t size, void *context) {
    return roaring_bitmap_aligned_malloc(alignment, size);
}

static void default_aligned_free(void *ptr, void *context) {
    roaring_bitmap_aligned_free(context);
}

static roaring_memory_t global_memory_hook = {
    .context = 0,
    .malloc = default_malloc,
    .realloc = default_realloc,
    .calloc = default_calloc,
    .free = default_free,
    .aligned_malloc = default_aligned_malloc,
    .aligned_free = default_aligned_free,
};

void roaring_init_memory_hook(roaring_memory_t memory_hook) {
    global_memory_hook = memory_hook;
}

void* roaring_malloc(size_t n) {
    return global_memory_hook.malloc(n, global_memory_hook.context);
}

void* roaring_realloc(void* p, size_t new_sz) {
    return global_memory_hook.realloc(p, new_sz, global_memory_hook.context);
}

void* roaring_calloc(size_t n_elements, size_t element_size) {
    return global_memory_hook.calloc(n_elements, element_size, global_memory_hook.context);
}

void roaring_free(void* p) {
    global_memory_hook.free(p, global_memory_hook.context);
}

void* roaring_aligned_malloc(size_t alignment, size_t size) {
    return global_memory_hook.aligned_malloc(alignment, size, global_memory_hook.context);
}

void roaring_aligned_free(void* p) {
    global_memory_hook.aligned_free(p, global_memory_hook.context);
}
