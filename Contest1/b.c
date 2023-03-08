#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

#ifdef DEBUG
#define memory_alloc(count, size) calloc(count, size)
#else
#define memory_alloc(count, size) malloc(count*size)
#endif

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        return -1;                           \
    }                                        \
}

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

int BinarySearch(const int *begin, const int *end, int target);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    unsigned int cnt = 0;
    panic_if_not(scanf("%u", &cnt), 1);

    int *input_array = (int *) memory_alloc(cnt, sizeof(int));

    for (unsigned int i = 0; i < cnt; ++i) {
        panic_if_not(scanf("%i", input_array + i), 1);
    }

    unsigned int beg = 0, end = 0;
    int val = 0;
    panic_if_not(scanf("%u", &cnt), 1);

    for (unsigned int i = 0; i < cnt; ++i) {
        panic_if_not(scanf("%u%u%d", &beg, &end, &val), 3);
        if (BinarySearch(input_array + beg, input_array + end, val)) {
            printf("YES\n");
        } else {
            printf("NO\n");
        }
    }

    free(input_array);

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

int BinarySearch(const int *begin, const int *end, int target) {
    if (end > begin) {
        end--;
    } else {
        return 0;
    }

    const int *mid = NULL;

    while (begin <= end) {
        mid = begin + (end - begin) / 2;

        if (*mid < target) {
            begin = mid + 1;
        } else if (*mid > target) {
            end = mid - 1;
        } else {
            return 1;
        }
    }

    return 0;
}
