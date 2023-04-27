#ifndef ALGO_BINARY_TREE_H
#define ALGO_BINARY_TREE_H

struct splay_node_t {
    splay_node_t *left;
    splay_node_t *right;

    int val;
};

splay_node_t *binary_insert(splay_node_t *self, int val);
splay_node_t *binary_delete(splay_node_t *self, int val);

#endif //ALGO_BINARY_TREE_H
