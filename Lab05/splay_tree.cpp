#include "splay_tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------

#define KEY(self) (self->key)

struct node_pair_t {
    struct splay_node_t *first;
    struct splay_node_t *second;
};

struct node_splay_return_t {
    struct splay_node_t *left_root;
    struct splay_node_t *left_max;
    struct splay_node_t *middle;
    struct splay_node_t *right_root;
    struct splay_node_t *right_min;
};

// ---------------------------------------------------------------------------------------------------------------------

#define check_input(action, expected_res)                                      \
    {                                                                          \
        if ((action) != expected_res) {                                        \
            fprintf (stderr, "Scanf error: Expected %d args\n", expected_res); \
            return -1;                                                         \
        }                                                                      \
    }

// ---------------------------------------------------------------------------------------------------------------------

// --- Tree ---


// --- Node ---
void node_free(struct splay_node_t *self);
struct splay_node_t *node_insert (struct splay_node_t *self, struct splay_node_t *elem);
struct splay_node_t *node_delete(struct splay_node_t *self, int key);
struct splay_node_t *node_find(struct splay_node_t *self, const char *key);

// --- Static ---
static struct node_splay_return_t splay(struct splay_node_t *self, int key);
static struct splay_node_t *node_break_left(struct splay_node_t *self, struct splay_node_t **left_max_ptr, struct splay_node_t **left_root_ptr);
static struct splay_node_t *node_break_right(struct splay_node_t *self, struct splay_node_t **right_min_ptr, struct splay_node_t **right_root_ptr);
static void node_merge_trees(struct node_splay_return_t splayres);
static struct splay_node_t *node_rotate_left(struct splay_node_t *self);
static struct splay_node_t *node_rotate_right(struct splay_node_t *self);

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Tree Implementation
// ---------------------------------------------------------------------------------------------------------------------

struct splay_tree_t *tree_new() {
    struct splay_tree_t *self = (struct splay_tree_t *) calloc(1, sizeof(struct splay_tree_t));
    assert(self->root == NULL);
    return self;
}

void tree_free(struct splay_tree_t *self) {
    if (self) {
        node_free(self->root);
        free(self);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void tree_insert(struct splay_tree_t *self, const int key) {
    splay_node_t *node = (splay_node_t *) calloc(1, sizeof(splay_node_t));
    node->key = key;
    self->root = node_insert(self->root, node);
}

void tree_delete(struct splay_tree_t *self, const int key) {
    self->root = node_delete(self->root, key);
}


// ---------------------------------------------------------------------------------------------------------------------
// Node Implementation
// ---------------------------------------------------------------------------------------------------------------------

void node_free(struct splay_node_t *self) {
    if (self) {
        node_free(self->left);
        node_free(self->right);
        free(self);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

struct splay_node_t *node_insert(struct splay_node_t *self, struct splay_node_t *elem) {
    struct node_splay_return_t splayres = splay(self, KEY(elem));

    elem->left = splayres.left_root;
    elem->right = splayres.right_root;

    return elem;
}

// ---------------------------------------------------------------------------------------------------------------------

struct splay_node_t *node_delete(struct splay_node_t *self, int key) {
    struct node_splay_return_t splayres = splay(self, key);

    if (!splayres.left_max) {
        return splayres.right_root;
    }

    struct node_splay_return_t leftsplayres = splay(self, splayres.left_max->key + 1);
    assert(leftsplayres.right_root == nullptr);
    assert(leftsplayres.left_root->right == nullptr);
    assert(leftsplayres.left_root == leftsplayres.left_max);

    leftsplayres.left_root = splayres.right_root;
    return leftsplayres.left_root;
}

// ---------------------------------------------------------------------------------------------------------------------
// Static
// ---------------------------------------------------------------------------------------------------------------------

static struct node_splay_return_t splay(struct splay_node_t *self, int key) {
    int cmp_res = 0;
    struct splay_node_t *right_min  = NULL;
    struct splay_node_t *left_max   = NULL;
    struct splay_node_t *right_root = NULL;
    struct splay_node_t *left_root  = NULL;

    while (self && (cmp_res = key - KEY(self)) != 0) {
        if (cmp_res < 0) {
            cmp_res = (self->left) ? key - KEY(self->left) : 0;
            if (cmp_res == 0) {
                self = node_break_right(self, &right_min, &right_root);
            } else if (cmp_res < 0) {
                self = node_rotate_right(self);
                self = node_break_right(self, &right_min, &right_root);
            } else {
                self = node_break_right(self,&right_min, &right_root);
                self = node_break_left(self, &left_max, &left_root);
            }
        } else {
            cmp_res = (self->right) ? key - KEY(self->right) : 0;

            if (cmp_res == 0) {
                self = node_break_left(self, &left_max, &left_root);
            } else if (cmp_res > 0) {
                self = node_rotate_left(self);
                self = node_break_left(self, &left_max, &left_root);
            } else {
                self = node_break_left(self, &left_max, &left_root);
                self = node_break_right(self, &right_min, &right_root);
            }
        }
    }

    struct node_splay_return_t ret = {left_root, left_max, self, right_root, right_min};
    return ret;
};

// ---------------------------------------------------------------------------------------------------------------------

static struct splay_node_t *node_rotate_left(struct splay_node_t *self) {
    struct splay_node_t *right = self->right;
    self->right = right->left;
    right->left = self;

    return right;
}

static struct splay_node_t *node_rotate_right(struct splay_node_t *self) {
    struct splay_node_t *left = self->left;
    self->left = left->right;
    left->right = self;

    return left;
}

// ---------------------------------------------------------------------------------------------------------------------

static struct splay_node_t *node_break_left(struct splay_node_t *self, struct splay_node_t **left_max_ptr, struct splay_node_t **left_root_ptr) {
    struct splay_node_t *right_subtree = self->right;
    self->right = NULL;

    if (*left_max_ptr == NULL) {
        *left_root_ptr = *left_max_ptr = self;
    } else {
        (*left_max_ptr)->right = self;
        *left_max_ptr = self;
    }

    return right_subtree;
}

static struct splay_node_t *node_break_right(struct splay_node_t *self, struct splay_node_t **right_min_ptr, struct splay_node_t **right_root_ptr) {
    struct splay_node_t *left_subtree = self->left;
    self->left = NULL;

    if (*right_min_ptr == NULL) {
        *right_root_ptr = *right_min_ptr = self;
    } else {
        (*right_min_ptr)->left = self;
        *right_min_ptr = self;
    }

    return left_subtree;
}

// ---------------------------------------------------------------------------------------------------------------------

static void node_merge_trees(struct node_splay_return_t splayres) {
    if (splayres.left_root) {
        splayres.left_max->right = splayres.middle->left;
        splayres.middle->left = splayres.left_root;
    }

    if (splayres.right_root) {
        splayres.right_min->left = splayres.middle->right;
        splayres.middle->right = splayres.right_root;
    }
}