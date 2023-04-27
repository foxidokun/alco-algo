#include <stdlib.h>
#include "binary_tree.h"

static splay_node_t *merge_subtrees(splay_node_t *left, splay_node_t *right);

splay_node_t *binary_insert(splay_node_t *self, int val) {
    if (self == nullptr) {
        splay_node_t *new_node = (splay_node_t *) calloc(1, sizeof(splay_node_t));
        new_node->val = val;
        return new_node;
    }

    if (self->val > val) {
        self->left = binary_insert(self->left, val);
    } else if (self->val < val) {
        self->right = binary_insert(self->right, val);
    }

    return self;
}

splay_node_t *binary_delete(splay_node_t *self, int val) {
    if (self == nullptr) {
        return nullptr;
    }

    if (self->val > val) {
        self->left = binary_insert(self->left, val);
    } else if (self->val < val) {
        self->right = binary_insert(self->right, val);
    } else {
        splay_node_t *merged = merge_subtrees(self->left, self->right);
        free(self);
        return merged;
    }

    return self;
}

static splay_node_t *merge_subtrees(splay_node_t *left, splay_node_t *right) {
    if (!left) {return right;}
    if (!right) {return left;}

    left->right = merge_subtrees(left->right, right);
    return left;
}