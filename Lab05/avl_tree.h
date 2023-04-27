#ifndef ALGO_AVL_TREE_H
#define ALGO_AVL_TREE_H

struct avl_node_t {
    int val;

    unsigned int depth;
    struct avl_node_t *left;
    struct avl_node_t *right;
};

struct avl_node_t *avl_insert (struct avl_node_t *self, int value);
struct avl_node_t *avl_delete (struct avl_node_t *self, int value);

#endif //ALGO_AVL_TREE_H
