#include "avl_tree.h"


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// defines
// ---------------------------------------------------------------------------------------------------------------------
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)


#define check_input(action, expected_res)                                      \
    {                                                                          \
        if ((action) != expected_res) {                                        \
            fprintf (stderr, "Scanf error: Expected %d args\n", expected_res); \
            return -1;                                                         \
        }                                                                      \
    }

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// ---------------------------------------------------------------------------------------------------------------------
// structs
// ---------------------------------------------------------------------------------------------------------------------

struct big_ret_res {
    avl_node_t *biggest;
    avl_node_t *head;
};



// ---------------------------------------------------------------------------------------------------------------------
// prototypes
// ---------------------------------------------------------------------------------------------------------------------

big_ret_res get_max_elem(avl_node_t *self);
int get_next (struct avl_node_t *self, int val);

// ---------------------------------------------------------------------------------------------------------------------

struct avl_node_t *rotate_left(struct avl_node_t *self);
struct avl_node_t *rotate_right(struct avl_node_t *self);

void free_tree (struct avl_node_t *self);

// ---------------------------------------------------------------------------------------------------------------------

struct avl_node_t *node_new (int val);
void node_free (struct avl_node_t *self);

// ---------------------------------------------------------------------------------------------------------------------

struct avl_node_t *balance_node (struct avl_node_t *self);

void fix_depth(struct avl_node_t *self);
int get_balance_factor(struct avl_node_t *self);

// ---------------------------------------------------------------------------------------------------------------------
// Tree realisation
// ---------------------------------------------------------------------------------------------------------------------

int get_next (struct avl_node_t *self, int val) {
    if (unlikely(self == NULL)) {
        return -1;
    }

    if (self->val < val) {
        if (self->right) {
            return get_next(self->right, val);
        } else {
            return -1;
        }
    } else if (self->val == val) {
        return val;
    } else { // self->val > val
        if (self->left == NULL) { return self->val; }

        int founded_val = get_next (self->left, val);
        return (founded_val == -1) ? self->val : founded_val;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

struct avl_node_t *avl_insert (struct avl_node_t *self, int value) {
    if (unlikely(self == NULL)) { // True only if tree is empty
        return node_new(value);
    }

    if (self->val == value) {
        return self;
    }

    if (self->val > value) {
        if (self->left == NULL) {
            self->left = node_new(value);
            self->depth++;
            return self;
        }

        self->left = avl_insert(self->left, value);
        self = balance_node (self);
    } else {
        if (self->right == NULL) {
            self->right = node_new(value);
            self->depth++;
            return self;
        }

        self->right = avl_insert(self->right, value);
        fix_depth (self);
        self = balance_node (self);
    }

    return self;
}

struct avl_node_t *avl_delete (struct avl_node_t *self, int value) {
    if (self == nullptr) {
        return nullptr;
    }

    if (self->val == value) {
        if (!self->left) {return self->right;}
        if (!self->left->right) {
            self->val = self->left->val;
            free(self->left);
            self->left = nullptr;
            fix_depth(self);
            self = balance_node(self);
            return self;
        }

        big_ret_res max_el = get_max_elem(self->left);
        self->left = max_el.head;
        self->val = max_el.biggest->val;
        free(max_el.biggest);
        fix_depth(self);
        self = balance_node(self);
    } else if (self->val > value) {
        self->left = avl_delete(self->left, value);
        fix_depth(self);
        self = balance_node(self);
    } else {
        self->right = avl_delete(self->right, value);
        fix_depth(self);
        self = balance_node(self);
    }

    return self;
}

// ---------------------------------------------------------------------------------------------------------------------

#define SIMPLE_ROTATE_FUNC(direction, other_direction)                  \
struct avl_node_t *rotate_##direction(struct avl_node_t *self) {                \
    struct avl_node_t *self_##other_direction = self->other_direction;      \
    self->other_direction = self_##other_direction->direction;          \
    self_##other_direction->direction = self;                           \
                                                                        \
    fix_depth(self);                                                    \
    fix_depth(self_##other_direction);                                  \
    return self_##other_direction;                                      \
}

//SIMPLE_ROTATE_FUNC(left,  right)
SIMPLE_ROTATE_FUNC(right, left)

struct avl_node_t *rotate_left(struct avl_node_t *self) {
    struct avl_node_t *self_right = self->right;
    self->right = self_right->left;
    self_right->left = self;
    fix_depth(self);
    fix_depth(self_right);
    return self_right;
}

#undef SIMPLE_ROTATE_FUNC


// ---------------------------------------------------------------------------------------------------------------------

struct avl_node_t *balance_node (struct avl_node_t *self) {
    int balance_fact = get_balance_factor(self);

    if (balance_fact == -2) {
        if (get_balance_factor(self->right) == 1) {
            self->right = rotate_right(self->right);
        }

        self = rotate_left (self);
    } else if (balance_fact == 2) {
        if (get_balance_factor(self->left) == -1) {
            self->left = rotate_left(self->left);
        }

        self = rotate_right(self);
    }

    return self;
}

// ---------------------------------------------------------------------------------------------------------------------

void free_tree (struct avl_node_t *self) {
    if (self->left)  { free_tree (self->left);  }
    if (self->right) { free_tree (self->right); }

    if (likely(self != NULL)) {
        node_free(self);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

struct avl_node_t *node_new (int val) {
    struct avl_node_t *self = (struct avl_node_t *) calloc(1, sizeof (struct avl_node_t));

    self->depth = 1;
    self->left  = NULL;
    self->right = NULL;
    self->val   = val;

    return self;
}

void node_free (struct avl_node_t *self) {
    free (self);
}

// ---------------------------------------------------------------------------------------------------------------------

void fix_depth(struct avl_node_t *self) {
    self->depth = MAX (self->left ? self->left->depth : 0,
                       self->right ? self->right->depth : 0) + 1;
}

// ---------------------------------------------------------------------------------------------------------------------

int get_balance_factor(struct avl_node_t *self) {
    int l_depth = self->left ? (int)self->left->depth : 0;
    int r_depth = self->right ? (int)self->right->depth : 0;
    return l_depth - r_depth;
}

// ---------------------------------------------------------------------------------------------------------------------

big_ret_res get_max_elem(avl_node_t *self) {
    if (!self->right->right) {
        avl_node_t *res = self->right;
        self->right = nullptr;
        fix_depth(self);
        self = balance_node(self);
        return { res, self};
    }

    big_ret_res res = get_max_elem(self->right);
    self->right = res.head;
    fix_depth(self);
    self = balance_node(self);
    res.head = self;

    return res;
}
