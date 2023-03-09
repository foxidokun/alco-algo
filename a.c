#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

const int CMD_BUF_SIZE = 64;

#define loop while (1)

#ifdef DEBUG
#define memory_alloc(count, size) calloc(count, size)
#else
#define memory_alloc(count, size) malloc(count *size)
#endif

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct stack_node {
    struct stack_node *next;
    int value;
};

struct stack {
    struct stack_node *top;
    size_t size;
};

enum ERRORS { WRONG_CMD = 1, NOT_INT, NOT_STRING_CMD };

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

void stack_init(struct stack *self);

void stack_push(struct stack *self, int val);

void stack_pop(struct stack *self);

void stack_back(struct stack *self);

void stack_size(struct stack *self);

void stack_clear(struct stack *self);

void stack_finish(struct stack *self);

// -------- Node ---------

struct stack_node *stack_node_ctor(struct stack_node *next, int val);

void stack_node_dtor(struct stack_node *self);

void stack_node_recursive_dtor(struct stack_node *current_node);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

#define route(word, action)                                                    \
    if (strcmp(cmd_buf, word) == 0) {                                          \
        action;                                                                \
    } else

int main() {
    char cmd_buf[CMD_BUF_SIZE];
    int value = 0;

    struct stack stk = {.top = NULL, .size = 0};
    stack_init(&stk);

    loop {
        if (scanf("%s", cmd_buf) != 1) {
            fprintf(stderr, "Wrong input");
            return NOT_STRING_CMD;
        }

        if (strcmp(cmd_buf, "push") == 0) {
            if (scanf("%i", &value) != 1) {
                fprintf(stderr, "Wrong input: not int");
                return NOT_INT;
            }

            stack_push(&stk, value);
        } else if (strcmp("pop", cmd_buf) == 0) {
            stack_pop(&stk);
        } else if (strcmp("back", cmd_buf) == 0) {
            stack_back(&stk);
        } else if (strcmp("size", cmd_buf) == 0) {
            stack_size(&stk);
        } else if (strcmp("clear", cmd_buf) == 0) {
            stack_clear(&stk);
        } else if (strcmp("exit", cmd_buf) == 0) {
            printf("bye\n");
            break;
        } else {
            fprintf(stderr, "Wrong input: unknown command");
            return WRONG_CMD;
        }
    }

    stack_finish(&stk);
    return 0;
}

#undef route

// ---------------------------------------------------------------------------------------------------------------------
// Stack Interface
// ---------------------------------------------------------------------------------------------------------------------

void stack_init(struct stack *self) {
    self->top = NULL;
    self->size = 0;
}

void stack_push(struct stack *self, int val) {
    self->top = stack_node_ctor(self->top, val);
    self->size++;

    printf("ok\n");
}

void stack_pop(struct stack *self) {
    if (self->top == NULL) {
        printf("error\n");
    } else {
        printf("%i\n", self->top->value);

        struct stack_node *next = self->top->next;
        stack_node_dtor(self->top);
        self->top = next;
        self->size--;
    }
}

void stack_back(struct stack *self) {
    if (self->top == NULL) {
        printf("error\n");
    } else {
        printf("%i\n", self->top->value);
    }
}

void stack_size(struct stack *self) { printf("%lu\n", self->size); }

void stack_clear(struct stack *self) {
    stack_node_recursive_dtor(self->top);
    self->top = NULL;
    self->size = 0;

    printf("ok\n");
}

void stack_finish(struct stack *self) { stack_node_recursive_dtor(self->top); }

// ---------------------------------------------------------------------------------------------------------------------
// Node Interface
// ---------------------------------------------------------------------------------------------------------------------

struct stack_node *stack_node_ctor(struct stack_node *next, int val) {
    struct stack_node *new_node =
        (struct stack_node *)memory_alloc(1, sizeof(struct stack_node));
    new_node->next = next;
    new_node->value = val;
    return new_node;
}

void stack_node_dtor(struct stack_node *self) { free(self); }

void stack_node_recursive_dtor(struct stack_node *current_node) {
    struct stack_node *next_node;

    while (current_node != NULL) {
        next_node = current_node->next;
        stack_node_dtor(current_node);
        current_node = next_node;
    }
}
