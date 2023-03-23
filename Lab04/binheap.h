#ifndef ALGO_BINHEAP_H
#define ALGO_BINHEAP_H

#include <stdlib.h>

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct heap_t {
    long int *data;

    size_t size;
    size_t capacity;
};


void heap_init(struct heap_t *self, size_t capacity);

void heap_free(struct heap_t *self);

void heap_insert(struct heap_t *self, long int val);

long int heap_get_min(struct heap_t *self);

void heap_extract_min(struct heap_t *self);

void heapify_array(struct heap_t *self, long int *data, size_t size);


#endif //ALGO_BINHEAP_H
