#include "binheap.h"

#include <string.h>

#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

#ifndef NDEBUG
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

const unsigned int CMD_BUF_SIZE = 64;

typedef unsigned int uint;

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        return -1;                           \
    }                                        \
}


// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void heap_sift_up(struct heap_t *self, size_t indx);

static void heap_sift_down(struct heap_t *self, size_t indx);

static void heap_swap(struct heap_t *self, size_t left, size_t right);

#ifndef NDEBUG
static int verify_heap(struct heap_t *self);
#endif

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

void heap_init(struct heap_t *self, size_t capacity) {
    self->data = (long int *) calloc(capacity, sizeof(long int));

    self->capacity = capacity;
    self->size = 0;
}

void heap_free(struct heap_t *self) {
    heap_assert (self);

    free(self->data);
}

// ---------------------------------------------------------------------------------------------------------------------

void heap_insert(struct heap_t *self, long int val) {
    heap_assert (self);

    if (self->size == self->capacity) {
        self->data = (long int *) realloc(self->data, 2 * self->capacity * sizeof(long int));
        self->capacity *= 2;
    }

    self->data[self->size] = val;
    self->size++;

    heap_sift_up(self, self->size - 1);

    heap_assert (self);
}

long int heap_get_min(struct heap_t *self) {
    heap_assert (self);

    return self->data[0];
}

void heap_extract_min(struct heap_t *self) {
    heap_assert (self);

    self->size--;

    if (self->size > 0) {
        heap_swap(self, 0, self->size);
        heap_sift_down(self, 0);
    }

    heap_assert (self);
}

void heapify_array(struct heap_t *self, long int *data, size_t size) {
    self->data = data;

    self->capacity = size;
    self->size = size;

    for (ssize_t i = size / 2 - 1; i >= 0; --i) {
        heap_sift_down(self, i);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------------------------------------------------

static void heap_sift_up(struct heap_t *self, size_t indx) {
    if (self->size < 2) {
        return;
    }

    while (indx > 0) {
        size_t parent_indx = (indx - 1) / 2;

        if (self->data[indx] < self->data[parent_indx]) {
            heap_swap(self, indx, parent_indx);

            indx = parent_indx;
        } else {
            return;
        }
    }
}

static void heap_sift_down(struct heap_t *self, size_t indx) {
    if (self->size < 2) {
        return;
    }

    size_t max_parent_indx = self->size / 2;
    while (indx < max_parent_indx) {
        size_t right_indx = indx * 2 + 2;
        size_t min_indx = indx * 2 + 1; // left by default

        if (right_indx < self->size && self->data[right_indx] < self->data[min_indx]) {
            min_indx = right_indx;
        }

        if (self->data[indx] < self->data[min_indx]) {
            return;
        }

        heap_swap(self, indx, min_indx);
        indx = min_indx;
    }
}

static void heap_swap(struct heap_t *self, size_t left, size_t right) {
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

static int verify_heap(struct heap_t *self) {
    check (self->size <= self->capacity && "Broken size");

    check (self->requests_index[self->data[0].request_id] == 0 && "Broken index");

    for (uint i = 1; i < self->size; ++i) {
        check (self->data[i].val > self->data[(i-1)/2].val && "Invalid order");
        check (self->requests_index[self->data[i].request_id] == i && "Broken index");
    }

    return 1;
}
#endif