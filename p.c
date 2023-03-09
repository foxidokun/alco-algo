#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

#ifndef NDEBUG
#define heap_assert(heap)                                                      \
    {                                                                          \
        if (!verify_heap(heap)) {                                              \
            fprintf(stderr, "Verification failed at line %d", __LINE__);       \
        }                                                                      \
    }
#else
#define heap_assert(heap) ((void)(0))
#endif

#ifndef NDEBUG
const unsigned int START_CAPACITY = 2;
#else
const unsigned int START_CAPACITY = 1024;
#endif

const unsigned int CMD_BUF_SIZE = 64;

#define loop while (1)

typedef unsigned int uint;

#define check_input(action, expected_res)                                      \
    {                                                                          \
        if ((action) != expected_res) {                                        \
            fprintf(stderr, "Scanf error: Expected %d args\n", expected_res);  \
            return -1;                                                         \
        }                                                                      \
    }

#define MIN(lhs, rhs) (((lhs) > (rhs)) ? (rhs) : (lhs))
#define MAX(lhs, rhs) ((lhs) < (rhs)) ? (rhs) : (lhs)

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct heap_elem_t {
    long int val;
    uint request_id;
};

struct heap_t {
    struct heap_elem_t *data;
    uint *requests_index;

    uint size;
    uint capacity;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

void heap_init(struct heap_t *self, uint capacity, uint n_requests);

void heap_free(struct heap_t *self);

void heap_insert(struct heap_t *self, long int val, uint id);

long int heap_get_min(struct heap_t *self);

void heap_extract_min(struct heap_t *self);

void heap_decrease_key_by_request(struct heap_t *self, uint id, long int delta);

static void heap_sift_up(struct heap_t *self, uint indx);

static void heap_sift_down(struct heap_t *self, uint indx);

static void heap_swap(struct heap_t *self, uint left, uint right);

#ifndef NDEBUG
static int verify_heap(struct heap_t *self);
#endif

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main() {
    struct heap_t heap;
    char cmd_buf[CMD_BUF_SIZE];
    long int val;

    uint n_cmd = 0;
    check_input(scanf("%u", &n_cmd), 1);
    heap_init(&heap, START_CAPACITY, n_cmd);

    for (uint i_cmd = 0; i_cmd < n_cmd; ++i_cmd) {
        check_input(scanf("%s", cmd_buf), 1);

        if (strcmp(cmd_buf, "insert") == 0) {
            check_input(scanf("%li", &val), 1);

            heap_insert(&heap, val, i_cmd);
        } else if (strcmp(cmd_buf, "getMin") == 0) {
            printf("%li\n", heap_get_min(&heap));
        } else if (strcmp(cmd_buf, "extractMin") == 0) {
            heap_extract_min(&heap);
        } else if (strcmp(cmd_buf, "decreaseKey") == 0) {
            uint pos;
            check_input(scanf("%u%li", &pos, &val), 2);
            heap_decrease_key_by_request(&heap, pos - 1, val);
        }
    }

    heap_free(&heap);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

void heap_init(struct heap_t *self, uint capacity, uint n_requests) {
    self->data =
        (struct heap_elem_t *)calloc(capacity, sizeof(struct heap_elem_t));
    self->requests_index = (uint *)calloc(n_requests, sizeof(uint));

    self->capacity = capacity;
    self->size = 0;
}

void heap_free(struct heap_t *self) {
    heap_assert(self);

    free(self->data);
    free(self->requests_index);
}

// ---------------------------------------------------------------------------------------------------------------------

void heap_insert(struct heap_t *self, long int val, uint id) {
    heap_assert(self);

    if (self->size == self->capacity) {
        self->data = (struct heap_elem_t *)realloc(
            self->data, 2 * self->capacity * sizeof(struct heap_elem_t));
        self->capacity *= 2;
    }

    self->data[self->size].val = val;
    self->data[self->size].request_id = id;
    self->requests_index[id] = self->size;
    self->size++;

    heap_sift_up(self, self->size - 1);

    heap_assert(self);
}

long int heap_get_min(struct heap_t *self) {
    heap_assert(self);

    return self->data[0].val;
}

void heap_extract_min(struct heap_t *self) {
    heap_assert(self);

    self->size--;

    if (self->size > 0) {
        heap_swap(self, 0, self->size);
        heap_sift_down(self, 0);
    }

    heap_assert(self);
}

void heap_decrease_key_by_request(struct heap_t *self, uint id,
                                  long int delta) {
    heap_assert(self);

    uint index = self->requests_index[id];
    assert(self->data[index].request_id == id &&
           "broken index, please contact technical support if error persists");

    self->data[index].val -= delta;
    heap_sift_up(self, index);

    heap_assert(self);
}

// ---------------------------------------------------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------------------------------------------------

static void heap_sift_up(struct heap_t *self, uint indx) {
    if (self->size < 2) {
        return;
    }

    while (indx > 0) {
        uint parent_indx = (indx - 1) / 2;

        if (self->data[indx].val < self->data[parent_indx].val) {
            heap_swap(self, indx, parent_indx);

            indx = parent_indx;
        } else {
            return;
        }
    }
}

static void heap_sift_down(struct heap_t *self, uint indx) {
    if (self->size < 2) {
        return;
    }

    uint max_parent_indx = self->size / 2;
    while (indx < max_parent_indx) {
        uint right_indx = indx * 2 + 2;
        uint min_indx = indx * 2 + 1; // left by default

        if (right_indx < self->size &&
            self->data[right_indx].val < self->data[min_indx].val) {
            min_indx = right_indx;
        }

        if (self->data[indx].val < self->data[min_indx].val) {
            return;
        }

        heap_swap(self, indx, min_indx);
        indx = min_indx;
    }
}

static void heap_swap(struct heap_t *self, uint left, uint right) {
    struct heap_elem_t tmp = self->data[left];
    self->data[left] = self->data[right];
    self->data[right] = tmp;

    uint lhs_id = self->data[left].request_id;
    uint rhs_id = self->data[right].request_id;

    uint id_tmp = self->requests_index[lhs_id];
    self->requests_index[lhs_id] = self->requests_index[rhs_id];
    self->requests_index[rhs_id] = id_tmp;
}

// ---------------------------------------------------------------------------------------------------------------------
// debug section
// ---------------------------------------------------------------------------------------------------------------------

#ifndef NDEBUG
#define check(cond)                                                            \
    {                                                                          \
        if (!(cond)) {                                                         \
            fprintf(stderr, "Condition " #cond " failed.");                    \
            return 0;                                                          \
        }                                                                      \
    }

static int verify_heap(struct heap_t *self) {
    check(self->size <= self->capacity && "Broken size");

    check(self->requests_index[self->data[0].request_id] == 0 &&
          "Broken index");

    for (uint i = 1; i < self->size; ++i) {
        check(self->data[i].val > self->data[(i - 1) / 2].val &&
              "Invalid order");
        check(self->requests_index[self->data[i].request_id] == i &&
              "Broken index");
    }

    return 1;
}
#endif