#ifndef ALGO_SORTS_H
#define ALGO_SORTS_H

#include <stdlib.h>

typedef void (*sort_func_t)(int* array, size_t len);

void bubble_sort(int *array, size_t len);
void selection_sort (int *array, size_t len);
void insertion_sort_unoptimised (int *array, size_t len);
void insertion_sort_optimised (int *array, size_t len);
void insertion_sort_binsearch (int *array, size_t len);

void qsort_median (int *array, size_t len);
void qsort_central (int *array, size_t len);
void qsort_random (int *array, size_t len);

void merge_sort (int *array, size_t len, int *buf, unsigned int small_buf_size);

template<unsigned int optimisation_switch_size>
void merge_sort (int *array, size_t len) {
    int *buf = (int *) calloc(len, sizeof(int));
    merge_sort(array, len, buf, optimisation_switch_size);
    free (buf);
}

void radix_sort(int *const array_orig, size_t len);

#endif //ALGO_SORTS_H
