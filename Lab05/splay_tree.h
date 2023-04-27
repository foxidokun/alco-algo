#ifndef ALGO_SPLAY_TREE_H
#define ALGO_SPLAY_TREE_H

struct splay_node_t {
    struct splay_node_t *left;
    struct splay_node_t *right;

    int key;
};

struct splay_tree_t {
    struct splay_node_t *root;
};

struct splay_tree_t *tree_new();
void tree_free(struct splay_tree_t *self);
void tree_insert(struct splay_tree_t *self, int key);
void tree_delete(struct splay_tree_t *self, const int key);

#endif //ALGO_SPLAY_TREE_H
