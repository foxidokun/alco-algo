#include <emmintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

#define loop while (1)

#ifdef DEBUG
#define memory_alloc(count, size) calloc(count, size)
#else
#define memory_alloc(count, size) malloc(count *size)
#endif

typedef unsigned int uint;

typedef int (*comparator_f)(const void *, const void *);

#define panic_if_not(action, expected_res)                                     \
    {                                                                          \
        if ((action) != expected_res) {                                        \
            return -1;                                                         \
        }                                                                      \
    }

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void cust_qsort(void *base, size_t count, size_t size,
                       comparator_f comp);

static int intcmp(const void *lhs, const void *rhs);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    uint n;
    panic_if_not(scanf("%u", &n), 1);

    int *array = calloc(n, sizeof(int));

    for (uint i = 0; i < n; ++i) {
        panic_if_not(scanf("%i", array + i), 1);
    }

    cust_qsort(array, n, sizeof(int), intcmp);

    for (uint i = 0; i < n; ++i) {
        printf("%i ", array[i]);
    }

    free(array);
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

#define _big_t uint32_t
#define _tmp_init_val 0

static void swap(void *__restrict a, void *__restrict b, size_t size) {
    _big_t *c_a_big = (_big_t *)a;
    _big_t *c_b_big = (_big_t *)b;
    _big_t tmp_big = _tmp_init_val;

    while (size >= sizeof(_big_t)) {
        tmp_big = *c_a_big;
        *c_a_big = *c_b_big;
        *c_b_big = tmp_big;

        c_a_big++;
        c_b_big++;
        size -= sizeof(_big_t);
    }

    char *c_a = (char *)c_a_big;
    char *c_b = (char *)c_b_big;
    char tmp = 0;

    while (size > 0) {
        tmp = *c_a;
        *c_a = *c_b;
        *c_b = tmp;

        c_a++;
        c_b++;
        size--;
    }
}

static void cust_qsort(void *base, size_t count, size_t size,
                       comparator_f comp) {
    char *const base_p = (char *)base;
    // Small array optimisations

    if (count == 1) {
        return;
    } else if (count == 2 && comp(base_p, base_p + size) > 0) {
        swap(base_p + size, base_p + size, size);
    } else if (count == 3) {
        if (comp(base_p, base_p + size) > 0) {
            swap(base_p, base_p + size, size);
        }

        if (comp(base_p + size, base_p + 2 * size) > 0) {
            swap(base_p + size, base_p + 2 * size, size);

            if (comp(base_p, base_p + size) > 0) {
                swap(base_p, base_p + size, size);
            }
        }
    }

    // Regular algorithm

    size_t lo = 0;
    size_t hi = count - 1;
    size_t pi = count / 2;

    while (hi > lo) {
        while (comp(base_p + lo * size, base_p + pi * size) < 0 && lo < count)
            lo++;
        while (comp(base_p + pi * size, base_p + hi * size) < 0 && hi > 0)
            hi--;

        if (lo <= hi) {
            swap(base_p + lo * size, base_p + hi * size, size);

            if (pi == lo)
                pi = hi;
            else if (pi == hi)
                pi = lo;

            lo++;
            hi--;
        }
    }

    if (hi > 0) {
        cust_qsort(base_p, hi + 1, size, comp);
    }

    if (lo < count - 1) {
        cust_qsort(base_p + lo * size, count - lo, size, comp);
    }
}

static int intcmp(const void *lhs, const void *rhs) {
    int lhs_val = *(const int *)lhs;
    int rhs_val = *(const int *)rhs;

    if (lhs_val > rhs_val) {
        return 1;
    } else if (lhs_val < rhs_val) {
        return -1;
    } else {
        return 0;
    }
}
