#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

#define loop while (1)

typedef unsigned int uint;

#define check_input(action, expected_res)                                     \
    {                                                                          \
        if ((action) != expected_res) {                                       \
            fprintf (stderr, "Scanf error: Expected %d args\n", expected_res);                                                                  \
            return -1;                                                         \
        }                                                                      \
    }

#define MIN_MACRO(lhs, rhs) (((lhs) > (rhs)) ? (rhs) : (lhs))
#define MAX_MACRO(lhs, rhs) ((lhs) < (rhs)) ? (rhs) : (lhs)

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct vector_t {
    uint *data;
    uint size;
    uint capacity;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

void vector_init(struct vector_t *self, uint capacity);

void vector_free(struct vector_t *self);

void vector_push(struct vector_t *self, uint val);

uint k_stat(uint *array, uint size, uint k);

uint partition(uint *array, uint size);

static void vector_resize(struct vector_t *self);

static inline void swap(uint *lhs, uint *rhs);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main() {
    uint n, k;
    uint a_min_two, a_min_one;
    check_input(scanf("%u%u%u%u", &n, &k, &a_min_two, &a_min_one), 4);

    srand(time(NULL));
    struct vector_t vec;
    vector_init(&vec, n);

    vector_push(&vec, a_min_two);
    vector_push(&vec, a_min_one);

    uint a_current;
    for (uint i = 2; i < n; ++i) {
        a_current = (a_min_one * 123 + a_min_two * 45);
        if (a_current >> 23) { // 2^23 ≈ 8M -> faster check
            a_current = a_current % 10004321;
        }

        vector_push(&vec, a_current);
        a_min_two = a_min_one;
        a_min_one = a_current;
    }

    printf("%u\n", k_stat(vec.data, n, k - 1));

    vector_free(&vec);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

void vector_init(struct vector_t *self, uint capacity) {
    self->size = 0;
    self->capacity = capacity;

    self->data = (uint *)calloc(capacity, sizeof(uint));
}

void vector_free(struct vector_t *self) { free(self->data); }

void vector_push(struct vector_t *self, uint val) {
    if (self->size == self->capacity) {
        vector_resize(self);
    }

    self->data[self->size++] = val;
}

// ---------------------------------------------------------------------------------------------------------------------
// k-stats
// ---------------------------------------------------------------------------------------------------------------------

uint k_stat(uint *array, uint size, uint k) {
    if (size == 2) {
        assert(k < 2);
        if (k == 0) {
            return MIN_MACRO(array[0], array[1]);
        } else {
            return MAX_MACRO(array[0], array[1]);
        }
    }

    uint pivot = partition(array, size);

    if (pivot > k) {
        return k_stat(array, pivot, k);
    } else if (pivot < k) {
        return k_stat(array + pivot + 1, size - pivot - 1, k - pivot - 1);
    } else {
        return array[pivot];
    }
}

uint partition(uint *array, uint size) {
    if (size == 1) {
        return 0;
    }

    uint indx = rand() % size;
    swap(array + indx, array + size - 1);
    uint pivot = array[size - 1];

    int i = -1;

    for (uint j = 0; j <= size - 2; j++) {
        if (array[j] < pivot) {
            i++;
            swap(array + i, array + j);
        }
    }

    swap(array + i + 1, array + size - 1);
    return (i + 1);
}

// ---------------------------------------------------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------------------------------------------------

static void vector_resize(struct vector_t *self) {
    self->data = realloc(self->data, 2 * self->capacity * sizeof(uint));
    self->capacity *= 2;
}

static inline void swap(uint *lhs, uint *rhs) {
    uint tmp = *rhs;
    *rhs = *lhs;
    *lhs = tmp;
}