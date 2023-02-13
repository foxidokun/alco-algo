#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static uint inversion_counter(uint *array, uint size);

static uint inversion_counter_inner_func(uint *array, uint *tmp_buf, uint size);

static uint merge(const uint *lhs, const uint *rhs, uint *output, uint left_size, uint right_size);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    uint n;
    panic_if_not(scanf("%u", &n), 1);

    uint *array = calloc(n, sizeof (uint));

    for (uint i = 0; i < n; ++i) {
        panic_if_not(scanf("%u", array + i), 1);
    }

    uint inv_cnt = inversion_counter(array, n);

    printf("%u\n", inv_cnt);

    free (array);
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

static uint inversion_counter(uint *array, uint size) {
    uint *tmp_buf = calloc(size, sizeof(uint));

    uint cnt = inversion_counter_inner_func(array, tmp_buf, size);

    free(tmp_buf);
    return cnt;
}

static uint inversion_counter_inner_func(uint *array, uint *tmp_buf, uint size) {
    //TODO optimize for size = 2

    if (size == 1) {
        return 0;
    }

    uint half_size = size / 2;
    uint inversion_cnt = inversion_counter_inner_func(array, tmp_buf, half_size) +
                         inversion_counter_inner_func(array + half_size, tmp_buf, size - half_size);


    inversion_cnt += merge(array, array + half_size, tmp_buf, half_size, size - half_size);
    memcpy(array, tmp_buf, size * sizeof(uint));

    return inversion_cnt;
}

static uint merge(const uint *lhs, const uint *rhs, uint *output, uint left_size, uint right_size) {
    size_t i = 0;
    size_t j = 0;

    uint inversion_cnt = 0;

    while (i < left_size && j < right_size) {
        if (lhs[i] < rhs[j]) {
            output[i + j] = lhs[i];
            i++;
        } else {
            output[i + j] = rhs[j];
            j++;
            inversion_cnt += left_size - i;

        }
    }

    if (i < left_size) {
        while (i < left_size) {
            output[i + j] = lhs[i];
            i++;
        }
    } else {
        while (j < right_size) {
            output[i + j] = rhs[j];
            j++;
        }
    }

    return inversion_cnt;
}