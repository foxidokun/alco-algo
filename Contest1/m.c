#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

const int CMD_BUF_SIZE = 64;

const int START_CAPACITY = 1024;

typedef unsigned int uint;

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        return -1;                           \
    }                                        \
}

#define MIN(lhs, rhs) (((lhs) > (rhs)) ? (rhs) : (lhs))
#define MAX(lhs, rhs) ((lhs) < (rhs)) ? (rhs) : (lhs)

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct minimax_t {
    uint *data;
    size_t size;
    size_t capacity;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

void minimax_init(struct minimax_t *self, size_t capacity);

void minimax_free(struct minimax_t *self);

uint minimax_get_min(struct minimax_t *self, int *error);

uint minimax_get_max(struct minimax_t *self, int *error);

uint minimax_extract_min(struct minimax_t *self, int *error);

uint minimax_extract_max(struct minimax_t *self, int *error);

void minimax_insert(struct minimax_t *self, uint val);

void minimax_clear(struct minimax_t *self);

static void siftUp(struct minimax_t *self, size_t index);

static void siftUpMax(struct minimax_t *self, size_t index);

static void siftUpMin(struct minimax_t *self, size_t index);

static void siftDown(struct minimax_t *self, size_t index);

static void siftDownMax(struct minimax_t *self, size_t index);

static void siftDownMaxEnd(struct minimax_t *self, size_t index);

static void siftDownMin(struct minimax_t *self, size_t index);

static void siftDownMinEnd(struct minimax_t *self, size_t index);

static void resize(struct minimax_t *self);

static void swap(struct minimax_t *self, size_t lhs, size_t rhs);

static unsigned char get_2_pow(size_t val);

static size_t get_min_grandchild(struct minimax_t *self, size_t index);

static size_t get_max_grandchild(struct minimax_t *self, size_t index);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main() {
    uint cmd_count = 0;
    panic_if_not(scanf("%u", &cmd_count), 1);
    char cmd[CMD_BUF_SIZE];

    struct minimax_t heap;
    minimax_init(&heap, START_CAPACITY);
    uint val = 0;
    int error;

    for (uint i = 0; i < cmd_count; ++i) {
        panic_if_not(scanf("%s", cmd), 1);

        if (strcmp(cmd, "insert") == 0) {
            panic_if_not(scanf("%u", &val), 1);

            minimax_insert(&heap, val);
            printf ("ok\n");
        } else if (strcmp(cmd, "extract_min") == 0) {
            val = minimax_extract_min(&heap, &error);
            if (error) {
                printf ("error\n");
            } else {
                printf ("%u\n", val);
            }
        } else if (strcmp(cmd, "get_min") == 0) {
            val = minimax_get_min(&heap, &error);
            if (error) {
                printf ("error\n");
            } else {
                printf ("%u\n", val);
            }
        }else if (strcmp(cmd, "extract_max") == 0) {
            val = minimax_extract_max(&heap, &error);
            if (error) {
                printf ("error\n");
            } else {
                printf ("%u\n", val);
            }
        } else if (strcmp(cmd, "get_max") == 0) {
            val = minimax_get_max(&heap, &error);
            if (error) {
                printf ("error\n");
            } else {
                printf ("%u\n", val);
            }
        } else if (strcmp(cmd, "size") == 0) {
            printf ("%zu\n", heap.size);
        } else if (strcmp(cmd, "clear") == 0) {
            minimax_clear(&heap);
            printf ("ok\n");
        }
    }

    minimax_free(&heap);
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

void minimax_init(struct minimax_t *self, size_t capacity) {
    self->size = 0;
    self->capacity = capacity;
    self->data = calloc(capacity + 1, sizeof(uint));
}

void minimax_free(struct minimax_t *self) {
    free(self->data);
}

// ---------------------------------------------------------------------------------------------------------------------


uint minimax_get_min(struct minimax_t *self, int *error) {
    if (self->size == 0) {
        *error = 1;
        return -1u;
    }

    *error = 0;
    return self->data[1];
}

uint minimax_get_max(struct minimax_t *self, int *error) {
    *error = 0;

    switch (self->size) {
        case 0:
            *error = 1;
            return -1u;
        case 1:
            return self->data[1];
        case 2:
            return self->data[2];
        default:
            return MAX(self->data[2], self->data[3]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint minimax_extract_min(struct minimax_t *self, int *error) {
    if (self->size == 0) {
        *error = 1;
        return -1u;
    }

    *error = 0;
    uint min_val = self->data[1];

    swap(self, 1, self->size);
    self->size--;
    siftDown(self, 1);

    return min_val;
}

uint minimax_extract_max(struct minimax_t *self, int *error) {
    uint max_index = 0;

    switch (self->size) {
        case 0:
            *error = 1;
            return -1u;
        case 1:
            max_index = 1;
            break;
        case 2:
            max_index = 2;
            break;
        default:
            max_index = (self->data[2] > self->data[3]) ? 2 : 3;
            break;
    }

    *error = 0;
    uint max_val = self->data[max_index];

    swap(self, max_index, self->size);
    self->size--;
    siftDown(self, max_index);

    return max_val;
}

// ---------------------------------------------------------------------------------------------------------------------

void minimax_insert(struct minimax_t *self, uint val) {
    if (self->size == self->capacity) {
        resize(self);
    }

    self->size++;
    self->data[self->size] = val;
    siftUp(self, self->size);
}

// ---------------------------------------------------------------------------------------------------------------------

void minimax_clear(struct minimax_t *self) {
    self->size = 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------------------------------------------------

// #####################################################################################################################
//  sift up
// #####################################################################################################################

static void siftUp(struct minimax_t *self, size_t index) {
    unsigned char pow = get_2_pow(index);

    if (pow % 2) {
        siftUpMax(self, index);
    } else {
        siftUpMin(self, index);
    }
}

static void siftUpMax(struct minimax_t *self, size_t index) {
    if (index == 1) { return; }

    if (self->data[index] < self->data[index / 2]) {
        swap(self, index, index / 2);
        siftUpMin(self, index / 2);
    } else if (self->data[index] > self->data[index / 4] && index > 3) {
        swap(self, index, index / 4);
        siftUpMax(self, index / 4);
    }
}

static void siftUpMin(struct minimax_t *self, size_t index) {
    if (index == 1) { return; }

    if (self->data[index] > self->data[index / 2]) {
        swap(self, index, index / 2);
        siftUpMax(self, index / 2);
    } else if (self->data[index] < self->data[index / 4]) {
        swap(self, index, index / 4);
        siftUpMin(self, index / 4);
    }
}

// #####################################################################################################################
//  sift down
// #####################################################################################################################

static void siftDown(struct minimax_t *self, size_t index) {
    unsigned char pow = get_2_pow(index);

    if (pow % 2) {
        siftDownMax(self, index);
    } else {
        siftDownMin(self, index);
    }
}

//------------------------------------------------------ min -----------------------------------------------------------

static void siftDownMin(struct minimax_t *self, size_t index) {
    if (4 * index + 3 > self->size) {
        siftDownMinEnd(self, index);
        return;
    }

    size_t min_grandchild = get_min_grandchild(self, index);

    if (self->data[index] < self->data[min_grandchild]) {
        return;
    }

    swap(self, index, min_grandchild);

    if (self->data[min_grandchild] > self->data[min_grandchild / 2]) {
        swap(self, min_grandchild, min_grandchild / 2);
    }

    siftDownMin(self, min_grandchild);
}

static void siftDownMinEnd(struct minimax_t *self, size_t index) {
    assert (4 * index + 3> self->size);

    if (index * 2 > self->size) {
        return;
    }

    size_t min_index = 2*index;
    if (min_index + 1 <= self->size && self->data[min_index+1] < self->data[min_index]) {
        min_index++;
    }

    for (size_t i = 4 * index; i <= self->size; ++i) {
        if (self->data[i] < self->data[min_index]) {
            min_index = i;
        }
    }

    if (self->data[index] > self->data[min_index]) {
        swap(self, index, min_index);

        if (min_index >= 4 * index && self->data[min_index] > self->data[min_index / 2]) {
            swap(self, min_index, min_index / 2);
        }
    }
}

//------------------------------------------------------ max -----------------------------------------------------------

static void siftDownMax(struct minimax_t *self, size_t index) {
    if (4 * index + 3 > self->size) {
        siftDownMaxEnd(self, index);
        return;
    }

    size_t max_grandchild = get_max_grandchild(self, index);

    if (self->data[index] > self->data[max_grandchild]) {
        return;
    }

    swap(self, index, max_grandchild);

    if (self->data[max_grandchild] < self->data[max_grandchild / 2]) {
        swap(self, max_grandchild, max_grandchild / 2);
    }

    siftDownMax(self, max_grandchild);
}

static void siftDownMaxEnd(struct minimax_t *self, size_t index) {
    assert (4 * index + 3 > self->size);

    if (index * 2 > self->size) {
        return;
    }

    size_t max_index = 2 * index;
    if (max_index + 1 <= self->size && self->data[max_index + 1] > self->data[max_index]) {
        max_index++;
    }

    for (size_t i = 4 * index; i <= self->size; ++i) {
        if (self->data[i] > self->data[max_index]) {
            max_index = i;
        }
    }

    if (self->data[index] < self->data[max_index]) {
        swap(self, index, max_index);

        if (max_index >= 4 * index && self->data[max_index] < self->data[max_index / 2]) {
            swap(self, max_index, max_index / 2);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

static void resize(struct minimax_t *self) {
    self->data = (uint *) realloc(self->data, (2 * self->capacity + 1) * sizeof(uint));
    self->capacity *= 2;
}

// ---------------------------------------------------------------------------------------------------------------------

static void swap(struct minimax_t *self, size_t lhs, size_t rhs) {
    uint tmp = self->data[rhs];
    self->data[rhs] = self->data[lhs];
    self->data[lhs] = tmp;
}

// ---------------------------------------------------------------------------------------------------------------------

static unsigned char get_2_pow(size_t val) {
    unsigned char cnt = 0;

    while (val >>= 1) {
        cnt++;
    }

    return cnt;
}

static size_t get_min_grandchild(struct minimax_t *self, size_t index) {
    size_t first_grandchild = index * 4;
    size_t min_index = first_grandchild;
    uint min_value = self->data[first_grandchild];

    uint latest_num = MIN (self->size - first_grandchild + 1, 4);

    for (uint i = 1; i < latest_num; ++i) {
        if (self->data[first_grandchild + i] < min_value) {
            min_value = self->data[first_grandchild + i];
            min_index = first_grandchild + i;
        }
    }

    return min_index;
}

static size_t get_max_grandchild(struct minimax_t *self, size_t index) {
    size_t first_grandchild = index * 4;
    size_t max_index = first_grandchild;
    uint max_value = self->data[first_grandchild];

    uint latest_num = MIN (self->size - first_grandchild + 1, 4);

    for (uint i = 1; i < latest_num; ++i) {
        if (self->data[first_grandchild + i] > max_value) {
            max_value = self->data[first_grandchild + i];
            max_index = first_grandchild + i;
        }
    }

    return max_index;
}