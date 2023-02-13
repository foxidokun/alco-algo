#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

const unsigned int START_CAPACITY = 2;
const unsigned int CMD_BUF_SIZE = 64;

#define loop while (1)

#ifdef DEBUG
#define memory_alloc(count, size) calloc(count, size)
#else
#define memory_alloc(count, size) malloc(count*size)
#endif

typedef unsigned int uint;

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        return -1;                           \
    }                                        \
}


#define MIN(lhs, rhs) ((lhs) > (rhs)) ? (rhs) : (lhs)
#define MAX(lhs, rhs) ((lhs) < (rhs)) ? (rhs) : (lhs)

#define INDX(val) ((val) % self->capacity)

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct pair
{
    int data;
    int known_min;
};

/// [ x x x x x x x x x x x]
/// ↑         ↑            ↑
/// head      separator    tail
/// |---- ----|------ -----|
///      1           2
/// 1) Known min = min (data[self ... sep])
/// 2) Known min = min (data[sep ... self])


struct queue {
    struct pair *data;
    uint size;
    uint capacity;
    uint head;
    uint tail;
    uint separator;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void queue_init(struct queue *self);

static void queue_finish(struct queue *self);

static void queue_push(struct queue *self, int val);

static int queue_pop(struct queue *self, int *error);

static int queue_front(struct queue *self, int *error);

static void queue_clear(struct queue *self);

static void queue_resize(struct queue *self);

static int queue_min(struct queue *self, int *error);

static void queue_move_sep(struct queue *self);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    struct queue queue = {.data = NULL, .capacity=0, .head=0, .tail=0, .size=0};
    queue_init(&queue);

    char cmd_buf[CMD_BUF_SIZE];
    int val, error;

    int n_cmd = 0;
    panic_if_not(scanf("%i", &n_cmd), 1);

    for (int i_cmd = 0; i_cmd < n_cmd; ++i_cmd) {
        panic_if_not (scanf("%s", cmd_buf), 1);

        if (strcmp(cmd_buf, "enqueue") == 0) {
            panic_if_not (scanf("%i", &val), 1);

            queue_push(&queue, val);
            printf("ok\n");
        } else if (strcmp(cmd_buf, "dequeue") == 0) {
            val = queue_pop(&queue, &error);

            if (error) {
                printf("error\n");
            } else {
                printf("%i\n", val);
            }
        } else if (strcmp(cmd_buf, "front") == 0) {
            val = queue_front(&queue, &error);

            if (error) {
                printf("error\n");
            } else {
                printf("%i\n", val);
            }
        } else if (strcmp(cmd_buf, "size") == 0) {
            printf ("%u\n", queue.size);
        } else if (strcmp(cmd_buf, "clear") == 0) {
            queue_clear(&queue);
            printf ("ok\n");
        } else if (strcmp(cmd_buf, "min") == 0) {
            val = queue_min(&queue, &error);

            if (error) {
                printf("error\n");
            } else {
                printf("%i\n", val);
            }
        }
    }

    queue_finish (&queue);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

static void queue_init(struct queue *self) {
    self->data = memory_alloc (START_CAPACITY, sizeof(struct pair));

    self->capacity = START_CAPACITY;
    self->tail = 0;
    self->head = 0;
}

static void queue_finish(struct queue *self) {
    free(self->data);
}

static void queue_push(struct queue *self, int val) {
    if (self->size == self->capacity) {
        queue_resize(self);
    }

    self->data[self->tail].data = val;

    if (INDX(self->tail - self->separator) > 0) {
        self->data[self->tail].known_min = MIN(val, self->data[INDX(self->tail - 1)].known_min);
    } else {
        self->data[self->tail].known_min = val;
        self->separator = self->tail;
    }

    self->tail = (self->tail + 1) % self->capacity;
    self->size++;
}

static int queue_pop(struct queue *self, int *error) {
    if (self->size == 0) {
        *error = 1;
        return 0;
    } else {
        *error = 0;
    }

    int val = self->data[self->head].data;
    self->size--;

    if (self->head == self->separator) {
        queue_move_sep (self);
    }

    self->head = (self->head + 1) % self->capacity;
    return val;
}

static int queue_front(struct queue *self, int *error) {
    if (self->size == 0) {
        *error = 1;
        return 0;
    } else {
        *error = 0;
    }

    return self->data[self->head].data;
}

static void queue_clear(struct queue *self) {
    self->size = 0;
    self->head = 0;
    self->tail = 0;
    self->separator = 0;
}

static void queue_resize(struct queue *self) {
    self->data = realloc(self->data, 2 * self->capacity * sizeof(struct pair));

    if (self->head >= self->tail) {
        if (self->separator >= self->head) {
            self->separator += self->capacity;
        }

        for (uint i = self->head; i < self->capacity; ++i) {
            self->data[i + self->capacity] = self->data[i];
        }

        self->head = self->head + self->capacity;
    }

    self->capacity <<= 1;
}

static int queue_min(struct queue *self, int *error) {
    if (self->size == 0) {
        *error = 1;
        return 0;
    } else {
        *error = 0;
    }


    return MIN(self->data[INDX(self->tail - 1)].known_min, self->data[self->head].known_min);
}


static void queue_move_sep(struct queue *self) {
    int min_val = self->data[INDX(self->tail - 1)].data;

    for (uint i = 0; i < self->size; ++i) {
        min_val = MIN (min_val, self->data[INDX(self->tail - i - 1)].data);
        self->data[INDX(self->tail - i - 1)].known_min = min_val;
    }

    self->separator = self->tail;
}
