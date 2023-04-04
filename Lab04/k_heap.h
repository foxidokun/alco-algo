#ifndef ALGO_K_HEAP_H
#define ALGO_K_HEAP_H

#include <stdlib.h>

struct k_heap_t {
    long int *data;

    size_t size;
    size_t capacity;

    uint depth;
};

void heap_init(struct k_heap_t *self, size_t capacity, uint depth);

void heap_free(struct k_heap_t *self);

void heap_insert(struct k_heap_t *self, long int val);

long int heap_get_min(struct k_heap_t *self);

void heap_extract_min(struct k_heap_t *self);

void heapify_array(struct k_heap_t *self, long int *data, size_t size);


#endif //ALGO_K_HEAP_H
