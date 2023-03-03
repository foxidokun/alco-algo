#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NDEBUG

#include <assert.h>

#ifndef NDEBUG

#include <errno.h>
#include "fibheap_graph_consts.c.h"

#endif

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

typedef unsigned int uint;

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        abort();                             \
    }                                        \
}

#define MIN(lhs, rhs) (((lhs) > (rhs)) ? (rhs) : (lhs))
#define MAX(lhs, rhs) ((lhs) < (rhs)) ? (rhs) : (lhs)

const uint START_DEG_CAPACITY = 16;

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct Node {
    int value;
    uint degree;
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

struct DegArray {
    struct Node **data;
    size_t capacity;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

// Allocators

void fibheap_init(struct FibHeap *self);

void fibheap_free(struct FibHeap *self);

struct Node *new_node(int value);

void free_node(struct Node *self);

// Methods

void fibheap_insert(struct FibHeap *self, int value);

void fibheap_consolidate(struct FibHeap *self);

void degarray_init(struct DegArray *self, size_t capacity);

void degarray_free(struct DegArray *self);

struct Node *degarray_extract(struct DegArray *self, size_t pos);

void degarray_put(struct DegArray *self, struct Node *node);

// Library functions
struct Node *merge_subtree(struct Node *lhs, struct Node *rhs);

void insert_near(struct Node *pos, struct Node *node);

// Debug graph dump
#ifndef NDEBUG

void graph_dump(struct FibHeap *self);

void dump_node(FILE *stream, struct Node *self);

FILE *get_dump_file();

#endif

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main() {

    struct FibHeap heap;
    fibheap_init(&heap);

    fibheap_insert(&heap, 228);
    fibheap_insert(&heap, 1);
    fibheap_insert(&heap, 2);
    fibheap_insert(&heap, 3);
    fibheap_insert(&heap, 4);

    graph_dump(&heap);
    fibheap_consolidate(&heap);

    graph_dump(&heap);
    return 0;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Heap Implementation
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Ctor/Dtor
// ---------------------------------------------------------------------------------------------------------------------
void fibheap_init(struct FibHeap *self) {
    self->size = 0;
    self->min = NULL;
    self->num_trees = 0;
}

/// Return initialized node with given value
struct Node *new_node(int value) {
    struct Node *new = calloc(1, sizeof(struct Node)); // DO NOT CHANGE TO MALLOC

    new->value = value;
    new->left = new;
    new->right = new;

    return new;
}


// ---------------------------------------------------------------------------------------------------------------------
// Insert
// ---------------------------------------------------------------------------------------------------------------------

void fibheap_insert(struct FibHeap *self, int value) {
    struct Node *new = new_node(value);

    if (self->min == NULL) {
        self->min = new;
        self->num_trees = 1;
        return;
    }

    // Update double linked list
    insert_near(self->min, new);

    if (self->min->value > value) {
        self->min = new;
    }

    self->size++;
    self->num_trees++;
}

// ---------------------------------------------------------------------------------------------------------------------
// Consolidate
// ---------------------------------------------------------------------------------------------------------------------

void fibheap_consolidate(struct FibHeap *self) {
    struct DegArray degs;
    degarray_init(&degs, START_DEG_CAPACITY);

    struct Node *cur_node = self->min;
    struct Node *conflicting_node = NULL;

    for (uint i = 0; i < self->num_trees;) {
        conflicting_node = degarray_extract(&degs, cur_node->degree);
        if (conflicting_node) {
            cur_node = merge_subtree(cur_node, conflicting_node);
            self->num_trees--;
        } else {
            degarray_put(&degs, cur_node);
            cur_node = cur_node->right;
            i++;
        }
    }

    degarray_free(&degs);
}

// ---------------------------------------------------------------------------------------------------------------------
// DegArray implementation
// ---------------------------------------------------------------------------------------------------------------------
void degarray_init(struct DegArray *self, size_t capacity) {
    self->capacity = capacity;
    self->data = calloc(capacity, sizeof(struct Node *));
}

void degarray_free(struct DegArray *self) {
    free(self->data);
}

struct Node *degarray_extract(struct DegArray *self, size_t pos) {
    if (pos >= self->capacity) {
        return NULL;
    } else {
        struct Node *node = self->data[pos];
        self->data[pos] = NULL;
        return node;
    }
}

void degarray_put(struct DegArray *self, struct Node *node) {
    if (node->degree >= self->capacity) {
        size_t current_byte_size = self->capacity * sizeof(struct Node *);
        self->data = realloc(self->data, 2 * current_byte_size);
        memset(self->data + current_byte_size, 0, current_byte_size);
    }

    self->data[node->degree] = node;
}

// ---------------------------------------------------------------------------------------------------------------------
// Graph dump
// ---------------------------------------------------------------------------------------------------------------------

#ifndef NDEBUG

void graph_dump(struct FibHeap *self) {
    FILE *stream = get_dump_file();
    fprintf(stream, HEADER);

    dump_node(stream, self->min);

    struct Node *top_node = self->min->right;
    struct Node *start_node = self->min;
    while (top_node != start_node) {
        dump_node(stream, top_node);
        top_node = top_node->right;
    }

    fprintf(stream, "}");
    fclose(stream);
}

#endif

// ---------------------------------------------------------------------------------------------------------------------
// Lib functions
// ---------------------------------------------------------------------------------------------------------------------

struct Node *merge_subtree(struct Node *lhs, struct Node *rhs) {
    if (lhs->value > rhs->value) {
        struct Node *tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }

    assert (lhs->value <= rhs->value && "Invalid logic");

    int lhs_empty = (lhs->child == NULL);

    if (lhs_empty) {
        lhs->child = rhs;
        lhs_empty = 1;
    } else {
        insert_near(lhs->child, rhs);
    }

    rhs->left->right = rhs->right; // Delete right from it's linked list
    rhs->right->left = rhs->left;
    lhs->degree++;

    assert (rhs->parent == NULL && "He left home without goodbye...");
    rhs->parent = lhs;

    if (lhs_empty) {
        rhs->left = rhs;
        rhs->right = rhs;
    }

    return lhs;
}

// ---------------------------------------------------------------------------------------------------------------------
void insert_near(struct Node *pos, struct Node *node) {
    node->left = pos;
    node->right = pos->right;
    pos->right->left = node;
    pos->right = node;
}

// ---------------------------------------------------------------------------------------------------------------------

#ifndef NDEBUG

FILE *get_dump_file() {
    static uint call_cnt;
    call_cnt++;

    char FILENAME_BUF[64];

    sprintf(FILENAME_BUF, "../dump/%u.grv", call_cnt);

    FILE *dump_file = fopen(FILENAME_BUF, "w");

    if (!dump_file) {
        printf("fopen error: %s\n", strerror(errno));
    }
    return dump_file;
}


/// Dump node with all childs
void dump_node(FILE *stream, struct Node *self) {
    fprintf(stream,
            "node_%p [label = \"{addr: %p|val: %i|deg: %u|left: %p|right: %p| child: %p|parent: %p}\", fillcolor=\"lightblue\", color=\"darkblue\"];\n",
            self, self, self->value, self->degree, self->left, self->right, self->child, self->parent);

    fprintf(stream, "node_%p -> node_%p;\n", self, self->right);
    fprintf(stream, "node_%p -> node_%p;\n", self, self->left);

    struct Node *child = self->child;
    if (child == NULL) {
        return;
    }

    uint deg = self->degree;
    for (uint i = 0; i < deg; ++i) {
        fprintf(stream, "node_%p -> node_%p;\n", self, child);

        if (self == child) {
            fprintf (stream, "node_%p_br [label = \"{BROKEN}\", fillcolor=\"lightblue\", color=\"darkblue\"];\n", self);
            fprintf(stream, "node_%p <-> node_%p_br;\n", self, self->right);
        }

        dump_node(stream, child);
        child = child->right;
    }
}

#endif