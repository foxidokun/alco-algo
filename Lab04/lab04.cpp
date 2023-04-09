#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

const int START_CAPACITY = 1024;

#include "binheap.h"
#include "fibheap.h"
#include "k_heap.h"

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

const int N_MEASURES = 5;

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

long int *gen_rand_array(size_t len);
int *array_dup(int *array);

void bench_binheap_sort ();
void bench_kheap_sort ();
void bench_fibheap_sort ();

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

int main() {
    srand(time(NULL));
//    bench_binheap_sort();
    bench_kheap_sort();
//    bench_fibheap_sort();
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

void bench_binheap_sort () {
    struct timeval start, stop;

    for (uint len = 100000; len < 10000000; len += 100000) {
        long long unsigned int elapsed_us = 0;
        heap_t heap;

        for (int n = 0; n < N_MEASURES; ++n) {
            long int *arr = gen_rand_array(len);
            gettimeofday(&start, NULL);

            heapify_array(&heap, arr, len);

            for (uint i = 0; i < len; ++i) {
                heap_extract_min(&heap);
            }

            gettimeofday(&stop, NULL);
            elapsed_us += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);

            heap_free(&heap);
        }

        printf ("Bin Heap, %d, %llu\n", len, elapsed_us / N_MEASURES);
        fflush(stdout);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void bench_kheap_sort () {
    struct timeval start, stop;

    for (int k = 2; k <= 2048; k*=2) {
        for (uint len = 5000000; len < 10000000; len += 100000) {
            long long unsigned int elapsed_us = 0;
            k_heap_t heap;

            for (int n = 0; n < N_MEASURES; ++n) {
                long int *arr = gen_rand_array(len);
                gettimeofday(&start, NULL);

                heapify_array(&heap, arr, len);

                gettimeofday(&stop, NULL);

                for (uint i = 0; i < len; ++i) {
                    heap_extract_min(&heap);
                }

                gettimeofday(&stop, NULL);
                elapsed_us += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);

                heap_free(&heap);
                free(arr);
            }

            printf("K Heap <%d>, %d, %llu\n", k, len, elapsed_us / N_MEASURES);
            fflush(stdout);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void bench_fibheap_sort () {
    struct timeval start, stop;

    for (uint len = 100000; len < 10000000; len += 100000) {
        long long unsigned int elapsed_us = 0;
        FibHeap heap;
        fibheap_init(&heap);

        for (int n = 0; n < N_MEASURES; ++n) {
            long int *arr = gen_rand_array(len);
            gettimeofday(&start, NULL);

            for (uint i = 0; i < len; ++i) {
                fibheap_insert(&heap, arr[i]);
            }

            for (uint i = 0; i < len; ++i) {
                fibheap_extract_min(&heap);
            }

            gettimeofday(&stop, NULL);
            elapsed_us += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);
        }

        printf ("Bin Heap, %d, %llu\n", len, elapsed_us / N_MEASURES);
        fflush(stdout);
    }
}


// ---------------------------------------------------------------------------------------------------------------------

long int *gen_rand_array(size_t len) {
    long int *array = (long int *) calloc(len, sizeof (long int));

    for (size_t i = 0; i < len; ++i) {
        array[i] = rand();
    }

    return array;
}
