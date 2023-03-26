#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "tests.h"

const int TEST_ARRAY_SIZE = 1000;

// ---------------------------------------------------------------------------------------------------------------------

#define R "\033[91m"
#define G "\033[92m"
#define D "\033[39m"

// ---------------------------------------------------------------------------------------------------------------------

#define _TEST(cond)                                             \
{                                                               \
    if (cond)                                                   \
    {                                                           \
        printf (R "Test FAILED: %s\n\n" D, #cond);              \
        failed++;                                               \
    }                                                           \
    else                                                        \
    {                                                           \
        printf (G "Test OK:     %s\n\n" D, #cond);              \
        success++;                                              \
    }                                                           \
}

#define _ASSERT(cond)                                                           \
{                                                                               \
    if (!(cond))                                                                \
    {                                                                           \
        printf (R "## Test Error: %s##\n" D, __func__);                         \
        printf ("Condition check failed: %s\n", #cond);                         \
        printf ("Test location: File: %s Line: %d\n", __FILE__, __LINE__);      \
        return -1;                                                              \
    }                                                                           \
}

// ---------------------------------------------------------------------------------------------------------------------

void test_sorts () {
    unsigned int success = 0;
    unsigned int failed  = 0;

    srand(time(NULL));

    printf ("Starting tests...\n\n");

    _TEST(test_sort_func(bubble_sort));
    _TEST(test_sort_func(insertion_sort_unoptimised));
    _TEST(test_sort_func(insertion_sort_optimised));
    _TEST(test_sort_func(insertion_sort_binsearch));
    _TEST(test_sort_func(selection_sort));

    _TEST(test_sort_func(qsort_central));
    _TEST(test_sort_func(qsort_median));
    _TEST(test_sort_func(qsort_random));

    _TEST(test_sort_func(merge_sort));

    _TEST(test_sort_func(radix_sort));

    printf ("Tests total: %u, failed %u, success: %u, success ratio: %3.1lf%%\n",
            failed + success, failed, success, success * 100.0 / (success + failed));
}

// ---------------------------------------------------------------------------------------------------------------------

int test_sort_func (sort_func_t func) {
    int array[TEST_ARRAY_SIZE];

    for (int i = 0; i < TEST_ARRAY_SIZE; ++i) {
        array[i] = rand();
    }

    func(array, TEST_ARRAY_SIZE);

    for (int i = 0; i < TEST_ARRAY_SIZE-1; ++i) {
        _ASSERT(array[i] < array[i+1]);
    }

    return 0;
}
