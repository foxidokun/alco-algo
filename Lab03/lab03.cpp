#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include "sorts.h"
#include "tests.h"

const int ERROR = -1;
const int ITERATION_NUM = 5;

long bench_sorting_algo (const int * orig_array, size_t len, sort_func_t sort_algo);
int *gen_rand_array(size_t len);

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------


int main () {
#ifdef TEST
    test_sorts();
#endif

    for (size_t len = 1000; len <= 1000000; len += 10000) {
        int *array = gen_rand_array(len);

        printf("Insertion Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, insertion_sort));
        printf("Selection Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, selection_sort));
        printf("Bubble Sort,%zu,%ld\n",    len, bench_sorting_algo(array, len, bubble_sort));
        fflush(stdout);

        free(array);
    }

    for (size_t len = 1000; len <= 10000000; len += 100000) {
        int *array = gen_rand_array(len);

        printf("Quick Median Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_median));
        printf("Quick Central Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_central));
        printf("Quick Random Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_random));

        printf("Merge Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort));
        printf("Merge Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, radix_sort));
        fflush(stdout);

        free (array);
    }
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/// Returns elapsed ms
long bench_sorting_algo (const int * orig_array, size_t len, sort_func_t sort_algo) {
    struct timeval start, stop;
    long elapsed_ms = 0;
    int *array = (int*) malloc(len * sizeof (int));

    for (int i = 0; i < ITERATION_NUM; ++i) {
        memcpy (array, orig_array, len * sizeof (int));\
        gettimeofday(&start, NULL);

        sort_algo(array, len);

        gettimeofday(&stop, NULL);
        elapsed_ms += (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;
    }

    free (array);

    elapsed_ms /= ITERATION_NUM;
    return elapsed_ms;
}

// ---------------------------------------------------------------------------------------------------------------------

int *gen_rand_array(size_t len) {
    int *array = (int *) calloc(len, sizeof (int));

    for (size_t i = 0; i < len; ++i) {
        array[i] = rand();
    }

    return array;
}