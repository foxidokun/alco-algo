#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include "sorts.h"
#include <algorithm>

//#define TEST

#ifdef TEST
#include "tests.h"
#else
#include "enkiTS/src/TaskScheduler.h"
#endif

const int ERROR = -1;
const int ITERATION_NUM = 5;

long bench_sorting_algo (const int * orig_array, size_t len, sort_func_t sort_algo);

int *gen_rand_array(size_t len);
int *gen_equal_array(size_t len);
int *gen_lot_same_increasing_array(size_t len);
int *gen_zebra_array(size_t len);
int *gen_u_shape_array(size_t len);

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

int main () {
#ifdef TEST
    test_sorts();
#else
//    enki::TaskScheduler g_TS;
//    g_TS.Initialize();

//    enki::TaskSet quadratic_sorts_tests(20, []( enki::TaskSetPartition range, uint32_t) {
//        for (size_t len = 1000 + range.start * 10000; len <= 10000 * range.end; len += 20000) {
//            int *array = gen_rand_array(len);
//
//            printf("Insertion Sort Op,%zu,%ld\n", len, bench_sorting_algo(array, len, insertion_sort_optimised));
//            printf("Insertion Sort Unop,%zu,%ld\n", len, bench_sorting_algo(array, len, insertion_sort_unoptimised));
//            printf("Insertion Sort BSearch,%zu,%ld\n", len, bench_sorting_algo(array, len, insertion_sort_binsearch));
////            printf("Selection Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, selection_sort));
////            printf("Bubble Sort,%zu,%ld\n",    len, bench_sorting_algo(array, len, bubble_sort));
//            fflush(stdout);
//
//            free(array);
//        }
//    });

//    enki::TaskSet nlogn_sorts_tests(20, []( enki::TaskSetPartition range, uint32_t) {
//        for (size_t len = 1000 + 100000 * 0; len <= 500000 * 20; len += 500000) {
//            int *array = gen_equal_array(len);
//
//            printf("Quick Median Sort :: EQUAL,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_median));
//            printf("Quick Central Sort :: EQUAL,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_central));
//            printf("Quick Random Sort :: EQUAL,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_random));
//
////            printf("Merge Sort #0,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<0>));
////            printf("Merge Sort #4,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<4>));
////            printf("Merge Sort #8,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<8>));
////            printf("Merge Sort #16,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<16>));
////            printf("Merge Sort #32,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<32>));
////            printf("Merge Sort #64,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<64>));
////            printf("Merge Sort #128,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<128>));
////            printf("Merge Sort #256,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<256>));
////            printf("Merge Sort #512,%zu,%ld\n", len, bench_sorting_algo(array, len, merge_sort<512>));
//
////            printf("Radix Sort,%zu,%ld\n", len, bench_sorting_algo(array, len, radix_sort));
//
//            fflush(stdout);
//            free (array);
//        }
//
//    for (size_t len = 1000 + 100000 * 0; len <= 500000 * 20; len += 500000) {
//        int *array = gen_lot_same_increasing_array(len);
//
//        printf("Quick Median Sort :: LOTSAME,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_median));
//        printf("Quick Central Sort :: LOTSAME,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_central));
//        printf("Quick Random Sort :: LOTSAME,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_random));
//
//        fflush(stdout);
//        free (array);
//    }
//
//    for (size_t len = 1000 + 100000 * 0; len <= 500000 * 20; len += 500000) {
//        int *array = gen_zebra_array(len);
//
//        printf("Quick Median Sort :: ZEBRA,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_median));
//        printf("Quick Central Sort :: ZEBRA,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_central));
//        printf("Quick Random Sort :: ZEBRA,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_random));
//
//        fflush(stdout);
//        free (array);
//    }

    for (size_t len = 1000 + 100000 * 0; len <= 50000 * 20; len += 50000) {
        int *array = gen_u_shape_array(len);

        printf("Quick Median Sort :: USHAPE,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_median));
        printf("Quick Central Sort :: USHAPE,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_central));
        printf("Quick Random Sort :: USHAPE,%zu,%ld\n", len, bench_sorting_algo(array, len, qsort_random));

        fflush(stdout);
        free (array);
    }
//    });

//    g_TS.AddTaskSetToPipe( &quadratic_sorts_tests );
//    g_TS.AddTaskSetToPipe( &nlogn_sorts_tests );

//    g_TS.WaitforAll();

#endif
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/// Returns elapsed ms
long bench_sorting_algo (const int * orig_array, size_t len, sort_func_t sort_algo) {
    struct timeval start, stop;
    long long unsigned int elapsed_us = 0;
    int *array = (int*) malloc(len * sizeof (int));

    for (int i = 0; i < ITERATION_NUM; ++i) {
        memcpy (array, orig_array, len * sizeof (int));\
        gettimeofday(&start, NULL);

        sort_algo(array, len);

        gettimeofday(&stop, NULL);
        elapsed_us += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);
    }

    free (array);

    elapsed_us /= ITERATION_NUM;
    return elapsed_us;
}

// ---------------------------------------------------------------------------------------------------------------------

int *gen_rand_array(size_t len) {
    int *array = (int *) calloc(len, sizeof (int));

    for (size_t i = 0; i < len; ++i) {
        array[i] = rand();
    }

    return array;
}

int *gen_equal_array(size_t len) {
    int *array = (int *) calloc(len, sizeof (int));

    for (size_t i = 0; i < len; ++i) {
        array[i] = 420228;
    }

    return array;
}

int *gen_lot_same_increasing_array(size_t len) {
    int *array = (int *) calloc(len, sizeof (int));

    for (size_t i = 0; i < len; ++i) {
        array[i] = (int) i / 8;
    }

    return array;
}

int *gen_zebra_array(size_t len) {
    int *array = (int *) calloc(len, sizeof (int));

    for (size_t i = 0; i < len; ++i) {
        array[i] = (int) i % 8;
    }

    return array;
}

int *gen_u_shape_array(size_t len) {
    int *array = (int *) calloc(len, sizeof (int));

    for (size_t i = 0; i < len; ++i) {
        array[i] = (int) std::max(i, len - i - 1);
    }

    return array;
}
