#include <stdlib.h>
#include <limits.h>
#include "skip_list.h"

//struct skip_node_t {
//    int key;
//    skip_node_t *next;
//    skip_node_t *down;
//};
//
//struct skip_list_t {
//    skip_node_t *root;
//};

skip_list_t *tree_new() {
    skip_list_t *self = (skip_list_t *) calloc(1, sizeof(skip_list_t));
    return self;
}

void tree_free(skip_list_t *self) {
    skip_node_t *root = self->root;

    while (root != nullptr) {
        skip_node_t *next_root = root->down;

        while(root != nullptr) {
            skip_node_t *next_in_list = root->next;
            free(root);
            root = next_in_list;
        }

        root = next_root;
    }
}


skip_node_t *node_insert(skip_node_t *self, int key) {
    if (!self) {
        skip_node_t *new_node = (skip_node_t *) calloc(1, sizeof(skip_node_t));
        new_node->key = key;
        return new_node;
    }

    while (self->next && self->next->key < key) {
        self = self->next;
    }

    skip_node_t *new_node = node_insert(self->down, key);
    if (new_node == nullptr) { return nullptr; }

    new_node->next = self->next;
    self->next = new_node;

    int coin_res = rand() % 2;
    if (!coin_res) { return nullptr; }

    skip_node_t* next_level_node = (skip_node_t *) calloc(1, sizeof(skip_node_t));
    next_level_node->key = key;
    next_level_node->down = new_node;

    return next_level_node;
}

void tree_insert(skip_list_t *self, int key) {
    skip_node_t *new_node = node_insert(self->root, key);

    if (new_node != nullptr) {
        skip_node_t *higher_root = (skip_node_t *) calloc(1, sizeof (skip_node_t));
        higher_root->down = self->root;
        higher_root->next = new_node;
        higher_root->key = INT_MIN;

        self->root = higher_root;
    }
}

void node_delete(skip_node_t *self, int key) {
    if (!self) {return;}

    while (self->next && self->key < key) {
        self = self->next;
    }

    if (self->next && self->next->key == key) {
        skip_node_t *del_node = self->next;
        self->next = del_node->next;

        free (del_node);
    }

    node_delete(self->down, key);
}

void tree_delete(skip_list_t *self, int key) {
    node_delete(self->root, key);

    if (self->root->next == nullptr) {
        skip_node_t *del_node = self->root;
        self->root = self->root->down;
        free(del_node);
    }
}