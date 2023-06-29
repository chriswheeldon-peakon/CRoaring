#include <roaring/lin_alloc.h>
#include <stdlib.h>
#include <string.h>

#define BOUNDS_CHECK(allocator, size)              \
    if (allocator->head + size > allocator->end) { \
        return NULL;                               \
    }

static void lin_alloc_realign(lin_alloc_t *allocator, size_t alignment) {
    size_t rem = (uintptr_t)allocator->head % alignment;
    if (rem != 0) {
        allocator->head += rem;
    }
}

void lin_alloc_init(lin_alloc_t *allocator, size_t size) {
    allocator->buf = malloc(size);
    memset(allocator->buf, 0, size);
    allocator->head = allocator->buf;
    allocator->end = allocator->buf + size;
}

void lin_alloc_destroy(lin_alloc_t *allocator) {
    free(allocator->buf);
    allocator->buf = 0;
    allocator->head = 0;
    allocator->end = 0;
}

void *lin_alloc_malloc(size_t size, void *context) {
    if (size == 0) {
        return NULL;
    }
    lin_alloc_t *allocator = context;
    lin_alloc_realign(allocator, sizeof(max_align_t));

    BOUNDS_CHECK(allocator, sizeof(size));
    memcpy(allocator->head, &size, sizeof(size));
    allocator->head += sizeof(size);

    BOUNDS_CHECK(allocator, size);
    void *dst = allocator->head;
    allocator->head += size;
    return dst;
}

void *lin_alloc_realloc(void *ptr, size_t size, void *context) {
    if (size == 0) {
        return NULL;
    }
    if (ptr == NULL) {
        return lin_alloc_malloc(size, context);
    }

    lin_alloc_t *allocator = context;
    size_t original_size;
    memcpy(&original_size, (void *)((uintptr_t)ptr - sizeof(size_t)),
           sizeof(size_t));

    if (original_size >= size) {
        return ptr;
    }
    if ((uintptr_t)ptr + original_size == allocator->head) {
        BOUNDS_CHECK(allocator, size - original_size);
        allocator->head += size - original_size;
        return ptr;
    }

    void *dst = lin_alloc_malloc(size, context);
    memcpy(dst, ptr, original_size);
    return dst;
}

void *lin_alloc_calloc(size_t count, size_t size, void *context) {
    void *dst = lin_alloc_malloc(size * count, context);
    memset(dst, 0, size * count);
    return dst;
}

void lin_alloc_free(void *ptr, void *context) { return; }

void *lin_alloc_aligned_malloc(size_t alignment, size_t size, void *context) {
    lin_alloc_t *allocator = context;

    BOUNDS_CHECK(allocator, sizeof(size));
    allocator->head += sizeof(size);

    lin_alloc_realign(allocator, alignment);

    BOUNDS_CHECK(allocator, size);
    void *dst = allocator->head;
    memcpy(allocator->head - sizeof(size), &size, sizeof(size));
    allocator->head += size;
    return dst;
}

void lin_alloc_aligned_free(void *ptr, void *context) { return; }