#ifndef ALGO_DEKART_TREE_H
#define ALGO_DEKART_TREE_H

struct dek_node_t {
    int key;
    int n_childs;
    int priority;

    struct dek_node_t *left;
    struct dek_node_t *right;
};

struct dek_tree_t {
    struct dek_node_t *root;
};

struct dek_tree_t *tree_new    ();
void           tree_insert (struct dek_tree_t *self, int key);
void           tree_remove (struct dek_tree_t *self, int key);
void tree_free(dek_tree_t *self);

#endif //ALGO_DEKART_TREE_H
