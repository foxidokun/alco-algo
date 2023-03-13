#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "sorts.h"

typedef size_t (*pivot_func_t)(int *array, size_t len);

static void qsort_custom (int *array, size_t len, pivot_func_t piv_func);

static void merge_sort (int *array, size_t len, int *buf);
static void merge_arrays(int *left, int *right, size_t left_len, size_t right_len, int *buf);
static void swap (int *array, size_t i, size_t j);

// ---------------------------------------------------------------------------------------------------------------------

void bubble_sort(int *array, size_t len) {
    for (size_t i = 0; i < len - 1; ++i) {
        for (size_t j = 0; j < len - i - 1; ++j) {
            if (array[j] > array[j+1]) {
                swap (array, j, j+1);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void selection_sort (int *array, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        size_t min = i;

        for (size_t j = i+1; j < len; ++j) {
            if (array[j] < array[min]) {
                min = j;
            }
        }

        swap(array, i, min);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void insertion_sort (int *array, size_t len) {
    for (size_t i = 1; i < len; ++i) {
        size_t j = i;
        while (j > 0 && array[j-1] > array[j]) {
            swap(array, j-1, j);
            j--;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void qsort_median (int *array, size_t len) {
    pivot_func_t pivfunc = [] (int *piv_arr, size_t piv_len) {
        size_t max = 0;
        size_t min = 0;

        size_t mid = piv_len/2;
        if (piv_arr[mid] < piv_arr[min])      {min = mid;}
        else if (piv_arr[mid] > piv_arr[max]) {max = mid;}

        size_t end = piv_len-1;
        if (piv_arr[end] < piv_arr[min])      {min = end;}
        else if (piv_arr[end] > piv_arr[max]) {max = end;}

        if (max == end) {
            return (min == 0) ? mid : 0;
        } else if (max == mid) {
            return (min == 0) ? max : 0;
        } else {
            return (min == mid) ? max : mid;
        }
    };

    qsort_custom(array, len, pivfunc);
}

// ---------------------------------------------------------------------------------------------------------------------

void qsort_central (int *array, size_t len) {
    pivot_func_t pivfunc = [] (int*, size_t piv_len) {
       return piv_len / 2;
    };

    qsort_custom(array, len, pivfunc);
}

// ---------------------------------------------------------------------------------------------------------------------

void qsort_random (int *array, size_t len) {
    pivot_func_t pivfunc = [] (int*, size_t piv_len) {
        return (rand() % piv_len);
    };

    qsort_custom(array, len, pivfunc);
}

// ---------------------------------------------------------------------------------------------------------------------

void merge_sort (int *array, size_t len) {
    int *buf = (int *) calloc(len, sizeof(int));
    merge_sort(array, len, buf);
    free (buf);
}

// ---------------------------------------------------------------------------------------------------------------------

void radix_sort(int *const array_orig, size_t len) {
    int *array = array_orig;
    int *buf = (int *) calloc(len, sizeof(int));
    int *tmp_ptr;
    uint counters[257]; // 256 bytes + 1 reserved for simpler logic

    for (uint step = 0; step < sizeof (int); ++step) {
        memset (counters, 0, 257 * sizeof (uint));

        for (uint i = 0; i < len; ++i) {
            counters[((unsigned char *)(array + i))[step] + 1]++; // +1 => 257 надо
        }

        assert (counters[0] == 0);

        for (int i = 2; i < 256; ++i) {
            counters[i] += counters[i-1];
        }

        for (uint i = 0; i < len; ++i) {
            buf[counters[((unsigned char *)(array + i))[step]]] = array[i];
            counters[((unsigned char *)(array + i))[step]]++;
        }

        tmp_ptr = array;
        array = buf;
        buf = tmp_ptr;
    }

    if (array != array_orig) {
        memcpy (array_orig, array, sizeof(int) * len);
    }

    free(buf);
}

// ---------------------------------------------------------------------------------------------------------------------
// lib functions
// ---------------------------------------------------------------------------------------------------------------------

static void qsort_custom (int *array, size_t len, pivot_func_t piv_func) {
    int *const base_p = array;

    // Small array optimisations
    if (len == 1) { return; }
    else if (len == 2) {
        if (array[0] > array[1]) {
            swap(array, 0, 1);
        }
        return;
    }

    // Regular algorithm

    size_t lo = 0;
    size_t hi = len - 1;
    size_t pi = piv_func(array, len);

    while (hi > lo) {
        while (array[lo] < array[pi] && lo < len) lo++;
        while (array[hi] > array[pi] && hi > 0)   hi--;

        if (lo < hi) {
            swap (array, lo, hi);

            if (pi == lo) pi = hi;
            else if (pi == hi) pi = lo;

            lo++;
            hi--;
        }
    }

    if (hi > 0) {
        qsort_custom (base_p, hi + 1, piv_func);
    }

    if (lo < len - 1) {
        qsort_custom(base_p + lo, len - lo, piv_func);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

static void merge_sort (int *array, size_t len, int *buf) {
    if (len == 1) { return; }

    if (len == 2) {
        if (array[0] > array[1]) {
            swap(array, 0, 1);
        }

        return;
    }

    size_t half = len/2;
    merge_sort(array, half, buf);
    merge_sort(array + half, len - half, buf);

    merge_arrays(array, array+half, half, len-half, buf);
    memcpy(array, buf, len * sizeof(int));
}

static void merge_arrays(int *left, int *right, size_t left_len, size_t right_len, int *buf) {
    size_t left_pos = 0;
    size_t right_pos = 0;

    while (left_pos < left_len && right_pos < right_len) {
        if (left[left_pos] < right[right_pos]) {
            buf[left_pos+right_pos] = left[left_pos];
            left_pos++;
        } else {
            buf[left_pos+right_pos] = right[right_pos];
            right_pos++;
        }
    }

    while (left_pos < left_len) {
        buf[left_pos+right_pos] = left[left_pos];
        left_pos++;
    }

    while (right_pos < right_len) {
        buf[left_pos+right_pos] = right[right_pos];
        right_pos++;
    }
}


// ---------------------------------------------------------------------------------------------------------------------

static void swap (int *array, size_t i, size_t j) {
    int tmp  = array[i];
    array[i] = array[j];
    array[j] = tmp;
}