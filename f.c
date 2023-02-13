#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        return -1;                           \
    }                                        \
}

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct speeds_t {
    int *values;
    int *min_left;
    int *min_right;
};

typedef unsigned int uint;

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

void speeds_init(struct speeds_t *self, size_t size);

void speeds_free(struct speeds_t *self);

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    uint cnt;
    panic_if_not(scanf("%u", &cnt), 1);

    struct speeds_t speeds;
    speeds_init(&speeds, cnt);

    int val;
    panic_if_not(scanf("%i", &val), 1);
    speeds.values[0] = val;
    speeds.min_left[0] = val;

    for (uint i = 1; i < cnt; ++i) {
        panic_if_not(scanf("%i", &val), 1);
        speeds.values[i] = val;

        if (val <= speeds.min_left[i - 1]) {
            speeds.min_left[i] = val;
        } else {
            speeds.min_left[i] = speeds.min_left[i - 1];
        }
    }

    speeds.min_right[cnt - 1] = speeds.values[cnt - 1];

    for (uint i = 1; i < cnt; ++i) {
        if (speeds.values[cnt - i - 1] < speeds.min_right[cnt - i]) {
            speeds.min_right[cnt - i - 1] = speeds.values[cnt - i - 1];
        } else {
            speeds.min_right[cnt - i - 1] = speeds.min_right[cnt - i];
        }
    }

    panic_if_not(scanf("%u", &cnt), 1);

    uint left, right = 0;

    for (uint i = 0; i < cnt; ++i) {
        panic_if_not(scanf("%u%u", &left, &right), 2);
        left--;
        right--;

        if (speeds.min_left[left] < speeds.min_right[right]) {
            printf("%i\n", speeds.min_left[left]);
        } else {
            printf("%i\n", speeds.min_right[right]);
        }
    }

    speeds_free(&speeds);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Static
// ---------------------------------------------------------------------------------------------------------------------

void speeds_init(struct speeds_t *self, size_t size) {
    self->values = calloc(size, sizeof(int));
    self->min_left = calloc(size, sizeof(int));
    self->min_right = calloc(size, sizeof(int));
}

void speeds_free(struct speeds_t *self) {
    free(self->values);
    free(self->min_left);
    free(self->min_right);
}
