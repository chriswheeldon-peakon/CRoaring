#ifndef ROARING_LIN_ALLOC_H
#define ROARING_LIN_ALLOC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *buf;
    uint8_t *head;
    uint8_t *end;
} lin_alloc_t;

void lin_alloc_init(lin_alloc_t *allocator, size_t size);

void lin_alloc_destroy(lin_alloc_t *allocator);

void *lin_alloc_malloc(size_t size, void *context);
void *lin_alloc_realloc(void *ptr, size_t size, void *context);
void *lin_alloc_calloc(size_t count, size_t size, void *context);
void lin_alloc_free(void *ptr, void *context);
void *lin_alloc_aligned_malloc(size_t alignment, size_t size, void *context);
void lin_alloc_aligned_free(void *ptr, void *context);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif