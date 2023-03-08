#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

const int CMD_BUF_SIZE = 64;

#define loop while (1)

#ifdef DEBUG
#define memory_alloc(count, size) calloc(count, size)
#else
#define memory_alloc(count, size) malloc(count*size)
#endif

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        return -1;                           \
    }                                        \
}

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct dyn_stack_node {
    struct dyn_stack_node *next;
    void *value;
};

struct dyn_stack {
    struct dyn_stack_node *top;
    size_t obj_size;
};

// -------- Array ---------

struct arr_stack {
    void *array;
    size_t size;
    size_t capacity;
    size_t obj_size;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

struct dyn_stack *dyn_stack_ctor(size_t obj_size);

int dyn_stack_push(struct dyn_stack *self, void *val);

int dyn_stack_pop(struct dyn_stack *self, void *val);

int dyn_stack_top(struct dyn_stack *self, void *val);

void dyn_stack_dtor(struct dyn_stack *self);

// -------- Array ---------

struct arr_stack *arr_stack_ctor(size_t size, size_t obj_size);

int arr_stack_push(struct arr_stack *self, void *val);

int arr_stack_pop(struct arr_stack *self, void *val);

int arr_stack_top(struct arr_stack *self, void *val);

void arr_stack_dtor(struct arr_stack *self);

// -------- Node ---------

struct dyn_stack_node *dyn_stack_node_ctor(struct dyn_stack_node *next, void *val, size_t obj_size);

void dyn_stack_node_dtor(struct dyn_stack_node *self);

void dyn_stack_node_recursive_dtor(struct dyn_stack_node *current_node);

// -------- Tasks ---------

void measure_task_one();

void measure_task_two();

void measure_task_three();

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

#define route(word, action)              \
    if (strcmp(cmd_buf, word) == 0) {    \
        action;                          \
    } else

int main() {
    measure_task_one();
    measure_task_two();
    measure_task_three();

    return 0;
}

#undef route

// ---------------------------------------------------------------------------------------------------------------------
// dyn_stack Interface
// ---------------------------------------------------------------------------------------------------------------------

struct dyn_stack *dyn_stack_ctor(size_t obj_size) {
    struct dyn_stack *self = calloc(1, sizeof(struct dyn_stack));
    self->obj_size = obj_size;
    self->top = NULL;

    return self;
}

int dyn_stack_push(struct dyn_stack *self, void *val) {
    self->top = dyn_stack_node_ctor(self->top, val, self->obj_size);

    return 0;
}

int dyn_stack_pop(struct dyn_stack *self, void *val) {
    if (self->top == NULL) { return 1; }

    memcpy(val, self->top + 1, self->obj_size);

    struct dyn_stack_node *next = self->top->next;
    dyn_stack_node_dtor(self->top);
    self->top = next;

    return 0;
}

int dyn_stack_top(struct dyn_stack *self, void *val) {
    if (self->top == NULL) { return 1; }

    memcpy(val, self->top + 1, self->obj_size);
    return 0;
}

void dyn_stack_dtor(struct dyn_stack *self) {
    dyn_stack_node_recursive_dtor(self->top);
    free(self);
}

// ---------------------------------------------------------------------------------------------------------------------
// Array Stack Interface
// ---------------------------------------------------------------------------------------------------------------------

struct arr_stack *arr_stack_ctor(size_t size, size_t obj_size) {
    struct arr_stack *self = calloc(1, sizeof(struct arr_stack));
    self->array = calloc(size, obj_size);
    self->size = 0;
    self->capacity = size;
    self->obj_size = obj_size;

    return self;
}

int arr_stack_push(struct arr_stack *self, void *val) {
    if (self->size == self->capacity) {
        self->array = realloc(self->array, 2 * self->capacity * self->obj_size);
        self->capacity <<= 1;
    }

    memcpy(self->array + self->obj_size * self->size, val, self->obj_size);
    self->size++;

    return 0;
}

int arr_stack_pop(struct arr_stack *self, void *val) {
    if (self->size == 0) { return 1; }

    self->size--;
    memcpy(val, self->array + self->obj_size * self->size, self->obj_size);

    if (4 * self->size < self->capacity) {
        self->array = realloc(self->array, self->capacity * self->obj_size / 2);
        self->capacity >>= 1;
    }

    return 0;
}

int arr_stack_top(struct arr_stack *self, void *val) {
    if (self->size == 0) { return 1; }

    memcpy(val, self->array + self->obj_size * self->size, self->obj_size);

    return 0;
}

void arr_stack_dtor(struct arr_stack *self) {
    free(self->array);
    free(self);
}


// ---------------------------------------------------------------------------------------------------------------------
// Node Interface
// ---------------------------------------------------------------------------------------------------------------------

struct dyn_stack_node *dyn_stack_node_ctor(struct dyn_stack_node *next, void *val, size_t obj_size) {
    struct dyn_stack_node *new_node = (struct dyn_stack_node *) memory_alloc(1,
                                                                             sizeof(struct dyn_stack_node) + obj_size);
    new_node->next = next;
    new_node->value = new_node + 1;
    memcpy(new_node->value, val, obj_size);
    return new_node;
}

void dyn_stack_node_dtor(struct dyn_stack_node *self) {
    free(self);
}

void dyn_stack_node_recursive_dtor(struct dyn_stack_node *current_node) {
    struct dyn_stack_node *next_node;

    while (current_node != NULL) {
        next_node = current_node->next;
        dyn_stack_node_dtor(current_node);
        current_node = next_node;
    }
}


void measure_task_one() {
    struct dyn_stack *dyn = dyn_stack_ctor(sizeof(int));
    struct arr_stack *arr = arr_stack_ctor(16, sizeof(int));

    srand(time(NULL));

    struct timeval start, stop;
    int val = rand();

    for (int i = 0; i < 1000000; ++i) {
        arr_stack_push(arr, &val);
    }

    gettimeofday(&start, NULL);

    for (int i = 0; i < 1000000; ++i) {
        arr_stack_pop(arr, &val);
    }


    for (int cnt = 0; cnt < 9; ++cnt) {
        for (int i = 0; i < 500000; ++i) {
            arr_stack_pop(arr, &val);
        }

        for (int i = 0; i < 250000; ++i) {
            arr_stack_push(arr, &val);
        }
    }

    gettimeofday(&stop, NULL);
    printf("Task 1: Array %li (us)\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));

    gettimeofday(&start, NULL);

    for (int i = 0; i < 1000000; ++i) {
        dyn_stack_push(dyn, &val);
    }

    for (int i = 0; i < 1000000; ++i) {
        dyn_stack_pop(dyn, &val);
    }


    for (int cnt = 0; cnt < 9; ++cnt) {
        for (int i = 0; i < 500000; ++i) {
            dyn_stack_pop(dyn, &val);
        }

        for (int i = 0; i < 250000; ++i) {
            dyn_stack_push(dyn, &val);
        }
    }

    gettimeofday(&stop, NULL);
    printf("Task 1: Linked list %li (us)\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));
}

void measure_task_two() {
    struct dyn_stack *dyn = dyn_stack_ctor(sizeof(int));
    struct arr_stack *arr = arr_stack_ctor(16, sizeof(int));

    srand(time(NULL));

    struct timeval start, stop;
    int val = rand();

    for (int i = 0; i < 1000000; ++i) {
        arr_stack_push(arr, &val);
    }

    gettimeofday(&start, NULL);

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 10000; ++j) {
            arr_stack_push(arr, &val);
        }
        for (int j = 0; j < 10000; ++j) {
            arr_stack_pop(arr, &val);
        }
    }

    for (int cnt = 0; cnt < 9; ++cnt) {
        for (int i = 0; i < 500000; ++i) {
            arr_stack_pop(arr, &val);
        }

        for (int i = 0; i < 250000; ++i) {
            arr_stack_push(arr, &val);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 10000; ++j) {
            arr_stack_push(arr, &val);
        }
        for (int j = 0; j < 10000; ++j) {
            arr_stack_pop(arr, &val);
        }
    }

    gettimeofday(&stop, NULL);
    printf("Task 2: Array %li (us)\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));

    gettimeofday(&start, NULL);

    for (int i = 0; i < 1000000; ++i) {
        dyn_stack_push(dyn, &val);
    }

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 10000; ++j) {
            dyn_stack_push(dyn, &val);
        }
        for (int j = 0; j < 10000; ++j) {
            dyn_stack_pop(dyn, &val);
        }
    }

    for (int cnt = 0; cnt < 9; ++cnt) {
        for (int i = 0; i < 500000; ++i) {
            dyn_stack_pop(dyn, &val);
        }

        for (int i = 0; i < 250000; ++i) {
            dyn_stack_push(dyn, &val);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 10000; ++j) {
            dyn_stack_push(dyn, &val);
        }
        for (int j = 0; j < 10000; ++j) {
            dyn_stack_pop(dyn, &val);
        }
    }

    gettimeofday(&stop, NULL);
    printf("Task 2: Linked list %li (us)\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));
}

void measure_task_three() {
    struct dyn_stack *dyn = dyn_stack_ctor(sizeof(int));
    struct arr_stack *arr = arr_stack_ctor(16, sizeof(int));

    srand(time(NULL));

    struct timeval start, stop;
    int val = rand();

    for (int i = 0; i < 1000000; ++i) {
        arr_stack_push(arr, &val);
    }

    gettimeofday(&start, NULL);

    for (int j = 0; j < 1000000; ++j) {
        if ((double) rand() / RAND_MAX >= 0.5) {
            arr_stack_push(arr, &val);
        } else {
            arr_stack_pop(arr, &val);
        }
    }

    gettimeofday(&stop, NULL);
    printf("Task 3: Array %li (us)\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));

    for (int i = 0; i < 1000000; ++i) {
        dyn_stack_push(dyn, &val);
    }

    gettimeofday(&start, NULL);

    for (int j = 0; j < 1000000; ++j) {
        if ((double) rand() / RAND_MAX >= 0.5) {
            dyn_stack_push(dyn, &val);
        } else {
            dyn_stack_pop(dyn, &val);
        }
    }

    gettimeofday(&stop, NULL);
    printf("Task 3: Linked list %li (us)\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));
}