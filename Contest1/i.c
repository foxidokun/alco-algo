#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

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

static void radix_sort(uint64_t *const array_orig, uint size);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    uint n;
    panic_if_not(scanf("%u", &n), 1);

    uint64_t *array = calloc(n, sizeof (uint64_t));

    for (uint i = 0; i < n; ++i) {
        panic_if_not(scanf("%lu", array + i), 1);
    }

    radix_sort (array, n);

    for (uint i = 0; i < n; ++i) {
        printf ("%lu\n", array[i]);
    }

    free (array);
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

const int N_STEPS = 64 / 8;

static void radix_sort(uint64_t *const array_orig, uint size) {
    uint64_t *array = array_orig;
    uint64_t *buf = calloc(size, sizeof(uint64_t));
    uint64_t *tmp_ptr;
    uint counters[257]; // 256 bytes + 1 reserved for simpler logic

    for (int step = 0; step < N_STEPS; ++step) {
        memset (counters, 0, 257 * sizeof (uint));

        for (uint i = 0; i < size; ++i) {
            counters[((unsigned char *)(array + i))[step] + 1]++; // +1 => 257 надо
        }

        assert (counters[0] == 0);

        for (int i = 2; i < 256; ++i) {
            counters[i] += counters[i-1];
        }

        for (uint i = 0; i < size; ++i) {
            buf[counters[((unsigned char *)(array + i))[step]]] = array[i];
            counters[((unsigned char *)(array + i))[step]]++;
        }

        tmp_ptr = array;
        array = buf;
        buf = tmp_ptr;
    }

    if (array != array_orig) {
        memcpy (array_orig, array, sizeof(uint64_t) * size);
    }

    free(buf);
}