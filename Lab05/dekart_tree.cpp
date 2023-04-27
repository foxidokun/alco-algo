#include "dekart_tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// defines
// ---------------------------------------------------------------------------------------------------------------------

#define check_input(action, expected_res)                                      \
    {                                                                          \
        if ((action) != expected_res) {                                        \
            fprintf (stderr, "Scanf error: Expected %d args\n", expected_res); \
            return -1;                                                         \
        }                                                                      \
    }

#define CMD_BUF_SIZE 64

// ---------------------------------------------------------------------------------------------------------------------
// structs
// ---------------------------------------------------------------------------------------------------------------------

struct split_return_t {
    struct dek_node_t *left;
    struct dek_node_t *right;
    bool exists;
};



// ---------------------------------------------------------------------------------------------------------------------
// prototypes
// ---------------------------------------------------------------------------------------------------------------------

bool           tree_exists (struct dek_tree_t *self, int key);
struct dek_node_t *tree_next   (struct dek_tree_t *self, int key);
struct dek_node_t *tree_prev   (struct dek_tree_t *self, int key);
struct dek_node_t *tree_kth    (struct dek_tree_t *self, int key);


struct dek_node_t *node_insert (struct dek_node_t *self, int key);
struct dek_node_t *node_remove (struct dek_node_t *self, int key);
bool           node_exists (struct dek_node_t *self, int key);
struct dek_node_t *node_next   (struct dek_node_t *self, int key);
struct dek_node_t *node_prev   (struct dek_node_t *self, int key);
struct dek_node_t *node_kth    (struct dek_node_t *self, int key);

struct dek_node_t *node_new(int key, int priority);

static struct split_return_t   node_split (struct dek_node_t *self, int key);
static struct dek_node_t *         node_merge (struct dek_node_t *left, struct dek_node_t *right);
static void update(struct dek_node_t *self);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Tree implementation
// ---------------------------------------------------------------------------------------------------------------------

struct dek_tree_t *tree_new() {
    struct dek_tree_t *self = (struct dek_tree_t *) calloc(1, sizeof (struct dek_tree_t));
    assert(self->root == nullptr);
    return self;
}

void tree_insert(struct dek_tree_t *self, int key) {
    self->root = node_insert(self->root, key);
}

void tree_remove(struct dek_tree_t *self, int key) {
    self->root = node_remove(self->root, key);
}

bool tree_exists(struct dek_tree_t *self, int key) {
    return node_exists(self->root, key);
}

struct dek_node_t *tree_next(struct dek_tree_t *self, int key) {
    return node_next(self->root, key);
}

struct dek_node_t *tree_prev(struct dek_tree_t *self, int key) {
    return node_prev(self->root, key);
}

struct dek_node_t *tree_kth(struct dek_tree_t *self, int key) {
    return node_kth(self->root, key);
}

void subtree_free(struct dek_node_t *self) {
    if (self->left)  { subtree_free (self->left);  }
    if (self->right) { subtree_free(self->right); }

    if (self != NULL) {
        free(self);
    }
}

void tree_free(dek_tree_t *self) {
    if (self->root) {
        subtree_free(self->root);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Node implementation
// ---------------------------------------------------------------------------------------------------------------------

struct dek_node_t *node_insert (struct dek_node_t *self, int key) {
    struct split_return_t split_res = node_split(self, key);

    if (split_res.exists) {
        return self;
    }

    struct dek_node_t *new_node = node_new(key, rand() % 100);
    split_res.right = node_merge(new_node, split_res.right);
    return node_merge(split_res.left, split_res.right);
}

// ---------------------------------------------------------------------------------------------------------------------

bool node_exists (struct dek_node_t *self, int key) {
    while (self != NULL) {
        if (self->key == key) {
            return true;
        }

        self = (self->key > key) ? self->left : self->right;
    }

    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

struct dek_node_t *node_next (struct dek_node_t *self, int key) {
    if (self == NULL) {
        return NULL;
    }

    if (self->key <= key) {
        return node_next(self->right, key);
    } else {
        struct dek_node_t *res = node_next(self->left, key);

        return ((res) ? res : self);
    }
}

struct dek_node_t *node_prev (struct dek_node_t *self, int key) {
    if (self == NULL) {
        return NULL;
    }

    if (self->key < key) {
        struct dek_node_t *res = node_prev(self->right, key);

        return ((res) ? res : self);
    } else {
        return node_prev(self->left, key);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

struct dek_node_t *node_kth(struct dek_node_t *self, int k) {
    if (self == NULL) {
        return NULL;
    }

    int l_childs = (self->left) ? self->left->n_childs : 0;

    if (k == l_childs) {
        return self;
    } else if (k < l_childs) {
        return node_kth(self->left, k);
    } else {
        return node_kth(self->right, k - l_childs - 1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

struct dek_node_t *node_remove (struct dek_node_t *self, int key) {
    if (self == NULL) {
        return NULL;
    }

    if (self->key == key) {
        return node_merge(self->left, self->right);
    }

    if (self->key > key) {
        self->left = node_remove(self->left, key);
        update(self);
    } else if (self->key < key) {
        self->right = node_remove(self->right, key);
        update(self);
    }

    return self;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------


const struct split_return_t EXIST_VAL = {NULL, NULL, true};

struct split_return_t node_split (struct dek_node_t *self, int k) {
    if (self == NULL) {
        struct split_return_t retval = {NULL, NULL, false};
        return retval;
    }

    if (self->key == k) {
        struct split_return_t retval = {NULL, NULL, true};
        return retval;
    } else if (self->key > k) {
        struct split_return_t ret = node_split(self->left, k);
        if (ret.exists) { return EXIST_VAL; }

        self->left = ret.right;
        update (self);

        struct split_return_t retval = {ret.left, self, false};
        return retval;
    } else {
        struct split_return_t ret = node_split(self->right, k);
        if (ret.exists) { return EXIST_VAL; }

        self->right = ret.left;
        update(self);

        struct split_return_t retval = {self, ret.right, false};
        return retval;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

struct dek_node_t *node_merge (struct dek_node_t *left, struct dek_node_t *right) {
    if (!left)  { return right; }
    if (!right) { return left;  }

    if (left->priority > right->priority) {
        left->right = node_merge(left->right, right);
        update (left);
        return left;
    } else {
        right->left = node_merge(left, right->left);
        update (right);
        return right;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

static void update(struct dek_node_t *self) {
    int left_nchilds  = (self->left) ? self->left->n_childs : 0;
    int right_nchilds = (self->right) ? self->right->n_childs : 0;

    self->n_childs = 1 + left_nchilds + right_nchilds;
}


// ---------------------------------------------------------------------------------------------------------------------

struct dek_node_t *node_new(int key, int priority) {
    struct dek_node_t *self = (struct dek_node_t *) calloc(1, sizeof(struct dek_node_t));
    self->key       = key;
    self->n_childs  = 1;
    self->priority  = priority;

    assert (self->left  == NULL);
    assert (self->right == NULL);

    return self;
}
