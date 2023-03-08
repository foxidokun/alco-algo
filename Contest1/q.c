#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG

#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

typedef unsigned int uint;

#define panic_if_not(action, expected_res) { \
    if ((action) != expected_res) {          \
        abort();                             \
    }                                        \
}

#define MIN(lhs, rhs) (((lhs) > (rhs)) ? (rhs) : (lhs))
#define MAX(lhs, rhs) ((lhs) < (rhs)) ? (rhs) : (lhs)

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct data_t {
    uint **dir_sorted_matrix;
    uint **rev_sorted_matrix;

    uint length;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------
void data_init(struct data_t *self, uint length, uint dir_sorted_height, uint rev_sorted_height);

void data_free(struct data_t *self);

void load_matrix(uint **matrix, uint height, uint length);

uint get_min_max_index(const uint *dir_sort, const uint *rev_sort, uint length);

uint find_suitable_pos(uint elem, uint pos, uint length, const uint *array);

static uint **allocate2D(uint height, uint width);

// ---------------------------------------------------------------------------------------------------------------------
// User Interface
// ---------------------------------------------------------------------------------------------------------------------

int main() {
    uint dir_sorted_height, rev_sorted_height, length;
    panic_if_not(scanf("%u%u%u", &dir_sorted_height, &rev_sorted_height, &length), 3);

    struct data_t data;
    data_init(&data, length, dir_sorted_height, rev_sorted_height);

    load_matrix(data.dir_sorted_matrix, dir_sorted_height, length);
    load_matrix(data.rev_sorted_matrix, rev_sorted_height, length);

    uint request_cnt = 0;
    panic_if_not(scanf("%u", &request_cnt), 1);

    uint dir_indx, rev_indx;

    for (uint i = 0; i < request_cnt; ++i) {
        panic_if_not (scanf("%u%u", &dir_indx, &rev_indx), 2);

        uint k = get_min_max_index(data.dir_sorted_matrix[dir_indx - 1], data.rev_sorted_matrix[rev_indx - 1], length);
        printf("%u\n", k + 1);
    }

    data_free(&data);
}

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------


void data_init(struct data_t *self, uint length, uint dir_sorted_height, uint rev_sorted_height) {
    self->dir_sorted_matrix = allocate2D(dir_sorted_height, length);
    self->rev_sorted_matrix = allocate2D(rev_sorted_height, length);

    self->length = length;
}

void data_free(struct data_t *self) {
    free(self->dir_sorted_matrix);
    free(self->rev_sorted_matrix);
}

// ---------------------------------------------------------------------------------------------------------------------

void load_matrix(uint **matrix, uint height, uint length) {
    uint tmp = 0;

    for (uint i = 0; i < height; i++) {
        for (uint j = 0; j < length; ++j) {
            panic_if_not(scanf("%u", &tmp), 1);

            matrix[i][j] = tmp;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint get_min_max_index(const uint *dir_sort, const uint *rev_sort, uint length) {
    uint left_pos = 0;
    uint right_pos = length - 1;

    if (dir_sort[0] > rev_sort[0]) {
        return 0;
    }

    if (dir_sort[length - 1] < rev_sort[length - 1]) {
        return length - 1;
    }

    while (right_pos - left_pos > 1) {
        uint middle_pos = (left_pos + right_pos) / 2;

        if (dir_sort[middle_pos] > rev_sort[middle_pos]) {
            right_pos = middle_pos;
        } else if (dir_sort[middle_pos] < rev_sort[middle_pos]) {
            left_pos = middle_pos;
        } else {
            return middle_pos;
        }
    }

    uint left_max  = MAX(dir_sort[left_pos], rev_sort[left_pos]);
    uint right_max = MAX(dir_sort[right_pos], rev_sort[right_pos]);

    return (left_max < right_max) ? left_pos : right_pos;
}

// ---------------------------------------------------------------------------------------------------------------------
// Lib functions
// ---------------------------------------------------------------------------------------------------------------------

static uint **allocate2D(uint height, uint width) {
    size_t array_size = height * (sizeof(uint *) + width * sizeof(uint));

    uint **buf = calloc(array_size, 1);
    buf[0] = (uint *) (buf + height);

    for (uint i = 1; i < height; ++i) {
        buf[i] = buf[i - 1] + width;
    }

    return buf;
}