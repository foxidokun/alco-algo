#ifndef ALGO_FIBHEAP_H
#define ALGO_FIBHEAP_H

#include <stdlib.h>

struct Node {
    int key;
    uint degree;
    int mark;
    struct Node *left;
    struct Node *right;
    struct Node *child;
    struct Node *parent;
};

struct FibHeap {
    struct Node *min;
    uint num_trees;
    size_t size;
};

void fibheap_init(struct FibHeap *self);

void fibheap_free(struct FibHeap *self);

struct Node *fibheap_insert(struct FibHeap *self, int value);

int fibheap_extract_min (struct FibHeap *self);

#endif //ALGO_FIBHEAP_H
