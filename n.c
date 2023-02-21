#include <stdio.h>
#include <stdlib.h>

#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

const unsigned int START_CAPACITY = 2;
const unsigned int CMD_BUF_SIZE = 64;

#define loop while (1)

#ifdef DEBUG
#define memory_alloc(count, size) calloc(count, size)
#else
#define memory_alloc(count, size) malloc(count*size)
#endif

typedef unsigned int uint;

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        return -1;                           \
    }                                        \
}


#define MIN(lhs, rhs) (((lhs) > (rhs)) ? (rhs) : (lhs))
#define MAX(lhs, rhs) ((lhs) < (rhs)) ? (rhs) : (lhs)

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct heap_t {
    uint *data;
    uint size;
    uint capacity;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

void heap_init(struct heap_t *self, uint capacity);

void heap_free(struct heap_t *self);

void heap_insert(struct heap_t *self, uint val);

void heap_insert_filling(struct heap_t *self, uint val);

void heap_insert_when_filled(struct heap_t *self, uint val);

uint heap_get_greatest(struct heap_t *self);

uint *heap_to_sorted_array(struct heap_t *self);

static void heap_sift_up(struct heap_t *self, uint indx);

static void heap_sift_down(struct heap_t *self, uint indx);

static void heap_swap(struct heap_t *self, uint left, uint right);

int intcmp(const void *lhs, const void *rhs);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main() {
    uint n, k;
    long long unsigned prev, x, y;
    panic_if_not (scanf("%u%u%llu%llu%llu", &n, &k, &prev, &x, &y), 5);

    struct heap_t heap;
    heap_init(&heap, k);

    uint a_current;
    for (uint i = 0; i < n; ++i) {
        a_current = (uint) ((x * prev + y) % 1073741824);

        heap_insert(&heap, a_current);
        prev = a_current;
    }

    uint *sorted = heap_to_sorted_array(&heap);

    for (uint i = 0; i < k; ++i) {
        printf("%u ", sorted[i]);
    }

    heap_free(&heap);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

void heap_init(struct heap_t *self, uint capacity) {
    self->data = (uint *) calloc(capacity, sizeof(uint));

    self->capacity = capacity;
    self->size = 0;
}

void heap_free(struct heap_t *self) {
    free(self->data);
}

// ---------------------------------------------------------------------------------------------------------------------

void heap_insert(struct heap_t *self, uint val) {
    if (self->size < self->capacity) {
        heap_insert_filling(self, val);
    } else {
        heap_insert_when_filled(self, val);
    }
}

void heap_insert_filling(struct heap_t *self, uint val) {
    assert (self->size < self->capacity && "Unexpected call");

    self->data[self->size] = val;
    heap_sift_up(self, self->size);
    self->size++;
}

void heap_insert_when_filled(struct heap_t *self, uint val) {
    assert (self->size == self->capacity && "Unexpected call");

    if (val > self->data[0]) {
        return;
    }

    self->data[0] = val;
    heap_sift_down(self, 0);
}

uint heap_get_greatest(struct heap_t *self) {
    return self->data[0];
}

uint *heap_to_sorted_array(struct heap_t *self) {
    qsort(self->data, self->size, sizeof(uint), intcmp);
    return self->data;
}

// ---------------------------------------------------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------------------------------------------------

static void heap_sift_up(struct heap_t *self, uint indx) {
    while (indx > 0) {
        uint parent_indx = (indx - 1) / 2;

        if (self->data[indx] > self->data[parent_indx]) {
            heap_swap(self, indx, parent_indx);

            indx = parent_indx;
        } else {
            return;
        }
    }
}

static void heap_sift_down(struct heap_t *self, uint indx) {
    uint max_indx = (self->size - 1) / 2;
    while (indx <= max_indx) {
        uint right_indx = indx * 2 + 2;
        uint max_indx = indx * 2 + 1; // left by default

        if (right_indx < self->size && self->data[right_indx] > self->data[max_indx]) {
            max_indx = right_indx;
        }

        if (self->data[indx] > self->data[max_indx]) {
            return;
        }

        heap_swap(self, indx, max_indx);
        indx = max_indx;
    }
}


static void heap_swap(struct heap_t *self, uint left, uint right) {
    uint tmp = self->data[left];
    self->data[left] = self->data[right];
    self->data[right] = tmp;
}

int intcmp(const void *lhs, const void *rhs) {
    int lhs_val = *(const int *) lhs;
    int rhs_val = *(const int *) rhs;

    if (lhs_val > rhs_val) {
        return 1;
    } else if (lhs_val < rhs_val) {
        return -1;
    } else {
        return 0;
    }
}