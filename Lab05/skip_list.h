#ifndef ALGO_SKIP_LIST_H
#define ALGO_SKIP_LIST_H

struct skip_node_t {
    int key;
    skip_node_t *next;
    skip_node_t *down;
};

struct skip_list_t {
    skip_node_t *root;
};


void tree_insert(skip_list_t *self, int key);
skip_list_t *tree_new();
void tree_free(skip_list_t *self);
void tree_delete(skip_list_t *self, int key);

#endif //ALGO_SKIP_LIST_H
