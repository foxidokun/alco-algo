#include "k_heap.h"

#include <string.h>

#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

#ifndef NDEBUG
#include "stdio.h"

#define heap_assert(heap) { \
        if (!verify_heap(heap)) { \
            fprintf (stderr, "Verification failed at line %d", __LINE__); \
        }   \
    }
#else
#define heap_assert(heap) ((void) (0))
#endif

#ifndef NDEBUG
const unsigned int START_CAPACITY = 2;
#else
const unsigned int START_CAPACITY = 1024;
#endif

typedef unsigned int uint;

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void heap_sift_up(struct k_heap_t *self, size_t indx);

static void heap_sift_down(struct k_heap_t *self, size_t indx);

static void heap_swap(struct k_heap_t *self, size_t left, size_t right);

#ifndef NDEBUG
static int verify_heap(struct k_heap_t *self);
#endif

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

void heap_init(struct k_heap_t *self, size_t capacity, uint depth) {
    self->data = (long int *) calloc(capacity, sizeof(long int));

    self->capacity = capacity;
    self->size = 0;
    self->depth = depth;
}

void heap_free(struct k_heap_t *self) {
    heap_assert (self);

    free(self->data);
}

// ---------------------------------------------------------------------------------------------------------------------

void heap_insert(struct k_heap_t *self, long int val) {
    heap_assert (self);

    if (self->size+1 == self->capacity) {
        self->data = (long int *) realloc(self->data, 2 * self->capacity * sizeof(long int));
        self->capacity *= 2;
    }

    self->size++;
    self->data[self->size] = val;

    heap_sift_up(self, self->size);

    heap_assert (self);
}

long int heap_get_min(struct k_heap_t *self) {
    heap_assert (self);

    return self->data[1];
}

void heap_extract_min(struct k_heap_t *self) {
    heap_assert (self);

    self->size--;

    if (self->size > 1) {
        heap_swap(self, 1, self->size+1);
        heap_sift_down(self, 1);
    }

    heap_assert (self);
}

void heapify_array (struct k_heap_t *self, long int *data, size_t size) {
    self->data = (long int *) calloc(size+1, sizeof(long int));

    self->capacity = size+1;
    self->size = size;

    memcpy(self->data+1, data, size * sizeof(long int));
        for (ssize_t i = size; i > 0; --i) {
        heap_sift_down(self, i);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------------------------------------------------

//static void heap_sift_up(struct k_heap_t *self, size_t indx) {
//    if (self->size < 2) {
//        return;
//    }
//
//    while (indx > 1) {
//        size_t parent_indx = indx / self->depth;
//
//        if (self->data[indx] < self->data[parent_indx]) {
//            heap_swap(self, indx, parent_indx);
//
//            indx = parent_indx;
//        } else {
//            return;
//        }
//    }
//}

static void heap_sift_up(struct k_heap_t *self, size_t indx) {
    if (self->size == 1) {
        return;
    }

    if (self->data[indx / self->depth] < self->data[indx]) {
        heap_swap(self, indx / self->depth, indx);
        heap_sift_up(self, indx / self->depth);
    }
}

static void heap_sift_down(struct k_heap_t *self, size_t indx) {
    size_t left_child = (indx-1) * self->depth + 2;
    size_t right_child = left_child+self->depth - 1;

    if (self->size < left_child) {
        return;
    }

    if (self->size < right_child) {
        right_child = self->size;
    }

    size_t min_child = left_child;

    for (size_t i = left_child+1; i <= right_child; ++i) {
        if (self->data[i] < self->data[min_child]) {
            min_child = i;
        }
    }

    if (self->data[min_child] < self->data[indx]) {
        heap_swap(self, indx, min_child);
        heap_sift_down(self, min_child);
    }
}

static void heap_swap(struct k_heap_t *self, size_t left, size_t right) {
    long int tmp = self->data[left];
    self->data[left] = self->data[right];
    self->data[right] = tmp;
}


// ---------------------------------------------------------------------------------------------------------------------
// debug section
// ---------------------------------------------------------------------------------------------------------------------

#ifndef NDEBUG
#define check(cond) { \
if (!(cond)) {        \
    fprintf (stderr, "Condition " #cond " failed.");                  \
    return 0;  \
}}

static int verify_heap(struct k_heap_t *self) {
    check (self->size <= self->capacity && "Broken size");

    for (uint i = 2; i < self->size; ++i) {
        check (self->data[i] > self->data[i/self->depth] && "Invalid order");
    }

    return 1;
}
#endif