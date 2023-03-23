#include "fibheap.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define NDEBUG
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

const uint START_DEG_CAPACITY  = 16;
const uint START_INDX_CAPACITY = 1024;

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct DegArray {
    struct Node **data;
    size_t capacity;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

// Allocators

struct Node *new_node(int value);

void free_node(struct Node *self);

void indxtable_init (struct IndxTable *self, size_t capacity);
void indxtable_free (struct IndxTable *self);

void indxtable_insert (struct IndxTable *self, struct Node *node, struct FibHeap *heap);

// Methods

int fibheap_get_min (struct FibHeap *self);

void fibheap_delete(struct FibHeap *self, struct Node* node);

void fibheap_clear(struct FibHeap *self);

void fibheap_merge (struct FibHeap *dest, struct FibHeap* src);
void fibheap_consolidate(struct FibHeap *self);

void degarray_init(struct DegArray *self, size_t capacity);

void degarray_free(struct DegArray *self);

struct Node *degarray_extract(struct DegArray *self, size_t pos);

void degarray_put(struct DegArray *self, struct Node *node);

// Library functions
struct Node *merge_subtree(struct Node *lhs, struct Node *rhs);

void extract_subtree (struct FibHeap *self, struct Node *node);

void insert_near(struct Node *pos, struct Node *node);

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
    self->min = nullptr;
    self->num_trees = 0;
}

/// Return initialized node with given key
struct Node *new_node(int value) {
    Node *new_node = (Node *) calloc(1, sizeof(struct Node)); // DO NOT CHANGE TO MALLOC

    new_node->key = value;
    new_node->left = new_node;
    new_node->right = new_node;

    return new_node;
}

void free_node(struct Node *self) {
    free (self);
}

// ---------------------------------------------------------------------------------------------------------------------
// Insert
// ---------------------------------------------------------------------------------------------------------------------

struct Node *fibheap_insert(struct FibHeap *self, int value) {
    struct Node *new_node_obj = new_node(value);

    if (self->min == NULL) {
        self->min = new_node_obj;
        self->num_trees = 1;
        self->size = 1;
        return new_node_obj;
    }

    // Update double linked list
    insert_near(self->min, new_node_obj);

    if (self->min->key > new_node_obj->key) {
        self->min = new_node_obj;
    }

    self->size++;
    self->num_trees++;

    return new_node_obj;
}

// ---------------------------------------------------------------------------------------------------------------------
// Extract Min
// ---------------------------------------------------------------------------------------------------------------------
int fibheap_extract_min (struct FibHeap *self) {
    int val = self->min->key;

    if (self->size == 1) {
        free_node(self->min);
        self->min = NULL;
        self->size = 0;
        self->num_trees = 0;

        return val;
    }

    if (self->min->child != NULL) {
        struct Node *min_child = self->min->child;
        struct Node *cur_node = self->min->child->right;

        min_child->parent = NULL;
        self->num_trees++;

        while (cur_node != min_child) {
            cur_node->parent = NULL;
            cur_node = cur_node->right;
            self->num_trees++;
        }

        struct Node *right_node = self->min->right;
        self->min->left->right = min_child;
        right_node->left = min_child->left;
        min_child->left->right = self->min->right;
        min_child->left = self->min->left;
    } else {
        self->min->left->right = self->min->right;
        self->min->right->left = self->min->left;
    }


    self->num_trees--;
    self->size--;
    struct Node *new_min_node = self->min->left;
    free_node(self->min);
    self->min = new_min_node;

    fibheap_consolidate (self);

    return val;
}

int fibheap_get_min (struct FibHeap *self) {
    return self->min->key;
}

// ---------------------------------------------------------------------------------------------------------------------
// Decrease Key
// ---------------------------------------------------------------------------------------------------------------------
void fibheap_set_key(struct FibHeap *self, struct Node* node, int val) {
    node->key = val;

    if (node->parent == NULL) {
        struct Node* min_node = self->min;
        struct Node* cur_node = self->min->right;

        for (uint i = 1; i < self->num_trees; ++i) {
            if (cur_node->key < min_node->key) {
                min_node = cur_node;
            }

            cur_node = cur_node->right;
        }

        self->min = min_node;

        return;
    }

    struct Node *parent = node->parent;
    extract_subtree(self, node);

    if (parent->mark) {
        extract_subtree(self, parent);
        parent->mark = 0;
    } else {
        parent->mark = 1;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Decrease Key
// ---------------------------------------------------------------------------------------------------------------------
void fibheap_merge(struct FibHeap *dest, struct FibHeap *src) {
    struct Node *srcmin = src->min;

    if (srcmin == NULL) {
        return;
    }

    src->min = NULL;

    dest->size += src->size;
    dest->num_trees += src->num_trees;
    src->size = 0;
    src->num_trees = 0;

    if (dest->min == NULL) {
        dest->min = srcmin;
        return;
    }

    struct Node *src_right_node = srcmin->right;
    struct Node *dst_right_node = dest->min->right;
    srcmin->right = dst_right_node;
    dest->min->right = src_right_node;
    dst_right_node->left = srcmin;
    src_right_node->left = dest->min;

    if (srcmin->key < dest->min->key) {
        dest->min = srcmin;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Consolidate
// ---------------------------------------------------------------------------------------------------------------------

void fibheap_delete(struct FibHeap *self, struct Node* node) {
    fibheap_set_key (self, node, INT_MIN);
    fibheap_extract_min(self);
}

// ---------------------------------------------------------------------------------------------------------------------
// Consolidate
// ---------------------------------------------------------------------------------------------------------------------

void fibheap_clear(struct FibHeap *self)
{
    self->size = 0;
    self->min = NULL;

    // TODO: fix memory leak
}

// ---------------------------------------------------------------------------------------------------------------------
// Consolidate
// ---------------------------------------------------------------------------------------------------------------------

void fibheap_consolidate(struct FibHeap *self) {
    struct DegArray degs;
    degarray_init(&degs, START_DEG_CAPACITY);

    struct Node *cur_node     = self->min;
    struct Node *new_min_node = self->min;
    struct Node *conflicting_node = NULL;

    uint num_trees = self->num_trees;

    for (uint i = 0; i < num_trees;) {
        conflicting_node = degarray_extract(&degs, cur_node->degree);
        if (conflicting_node) {
            cur_node = merge_subtree(cur_node, conflicting_node);
            self->num_trees--;
        } else {
            degarray_put(&degs, cur_node);
            cur_node = cur_node->right;
            i++;
        }

        if (cur_node->key < new_min_node->key) {
            new_min_node = cur_node;
        }
    }

    self->min = new_min_node;

    degarray_free(&degs);
}

// ---------------------------------------------------------------------------------------------------------------------
// DegArray implementation
// ---------------------------------------------------------------------------------------------------------------------
void degarray_init(struct DegArray *self, size_t capacity) {
    self->capacity = capacity;
    self->data = (Node**) calloc(capacity, sizeof(struct Node *));
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
        self->data = (Node **) realloc(self->data, 2 * current_byte_size);
        memset(self->data + current_byte_size, 0, current_byte_size);
    }

    self->data[node->degree] = node;
}

// ---------------------------------------------------------------------------------------------------------------------
// Lib functions
// ---------------------------------------------------------------------------------------------------------------------

struct Node *merge_subtree(struct Node *lhs, struct Node *rhs) {
    if (lhs->key > rhs->key) {
        struct Node *tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }

    assert (compare_keys(lhs->key, rhs->key) < 0 && "Invalid logic");

    int lhs_empty = (lhs->child == NULL);

    rhs->left->right = rhs->right; // Delete right from it's linked list
    rhs->right->left = rhs->left;

    if (lhs_empty) {
        lhs->child = rhs;
        lhs_empty = 1;
    } else {
        insert_near(lhs->child, rhs);
    }

    lhs->degree++;

    assert (rhs->parent == NULL && "He left home without saying goodbye...");
    rhs->parent = lhs;

    if (lhs_empty) {
        rhs->left = rhs;
        rhs->right = rhs;
    }

    return lhs;
}

// ---------------------------------------------------------------------------------------------------------------------
void extract_subtree (struct FibHeap *self, struct Node *node) {
    assert (node->parent != NULL && "can't extract top subtree");

    if (node->parent->child == node) {
        if (node->parent->degree > 1) {
            node->parent->child = node->left;
        } else {
            node->parent->child = NULL;
        }
    }

    struct Node *right_node = node->right;
    node->left->right = node->right;
    right_node->left = node->left;

    self->min->right->left = node;
    node->right = self->min->right;
    node->left = self->min;
    self->min->right = node;

    if (self->min->key > node->key) {
        self->min = node;
    }

    node->parent->degree--;
    node->parent = NULL;
}

// ---------------------------------------------------------------------------------------------------------------------
void insert_near(struct Node *pos, struct Node *node) {
    node->left = pos;
    node->right = pos->right;
    pos->right->left = node;
    pos->right = node;
}

