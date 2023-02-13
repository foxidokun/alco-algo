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

int check(const int *points, unsigned int n, unsigned int max_k, unsigned int l);

int intcmp(const void *lhs, const void *rhs);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    unsigned int n = 0, k = 0;

    panic_if_not (scanf("%u", &n), 1);
    panic_if_not (scanf("%u", &k), 1);

    int *points = (int *) memory_alloc(n, sizeof (int));

    for (unsigned int i = 0; i < n; ++i)
    {
        panic_if_not(scanf("%i", points+i), 1);
    }

    qsort(points, n, sizeof (int), intcmp);

    unsigned int min = 0;
    unsigned int max = (unsigned) (points[n-1] - points[0]);
    unsigned int mid;

    while (min < max) {
        mid = min + (max - min) / 2;

        if (check(points, n, k, mid)) {
            max = mid;
        } else {
            min = mid + 1;
        }
    }

    printf("%u\n", max);

    free (points);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

int check(const int *points, const unsigned n, const unsigned int max_k, const unsigned int l) {
    unsigned int cnt = 1;
    int last_begin = points[0];

    for (unsigned i = 0; i < n; ++i) {
        if ((unsigned) (points[i] - last_begin) <= l) {
            continue;
        }

        cnt++;
        last_begin = points[i];

        if (cnt > max_k) {
            return 0;
        }
    }

    return 1;
}

int intcmp(const void *lhs, const void *rhs)
{
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
