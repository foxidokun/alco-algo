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
typedef unsigned int uint;

struct segments_t {
    uint *begins;
    uint *ends;
    size_t size;
    size_t unique_cnt;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void segments_init(struct segments_t *self, size_t size);

static void segments_free(struct segments_t *self);

static void segments_sort(struct segments_t *self);

static void segments_unite(struct segments_t *self);

static int intcmp(const void *lhs, const void *rhs);

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

int main(void) {
    uint cnt;
    panic_if_not(scanf("%u", &cnt), 1);

    struct segments_t segments;
    segments_init(&segments, cnt);

    for (uint i = 0; i < cnt; ++i) {
        panic_if_not(scanf("%u%u", segments.begins + i, segments.ends + i), 2);
    }

    segments_sort(&segments);
    segments_unite(&segments);

    printf("%lu\n", segments.unique_cnt);
    uint beg_indx = 0;
    uint end_indx = 0;

    while (end_indx < segments.size) {
        while (segments.begins[beg_indx] == 0) { beg_indx++; }
        while (segments.ends  [end_indx] == 0) { end_indx++; }

        printf ("%u %u\n", segments.begins[beg_indx++], segments.ends[end_indx++]);
    }

    segments_free(&segments);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Static
// ---------------------------------------------------------------------------------------------------------------------

static void segments_init(struct segments_t *self, size_t size) {
    self->begins = calloc(size, sizeof(uint));
    self->ends = calloc(size, sizeof(uint));
    self->size = size;
    self->unique_cnt = size;
}

static void segments_free(struct segments_t *self) {
    free(self->begins);
    free(self->ends);
}

static void segments_sort(struct segments_t *self) {
    qsort(self->begins, self->size, sizeof(uint), intcmp);
    qsort(self->ends, self->size, sizeof(uint), intcmp);
}


static void segments_unite(struct segments_t *self) {
    uint depth_cnt = 0;

    uint beg_indx = 0;
    uint end_indx = 0;

    while (beg_indx < self->size && end_indx < self->size) {
        if (self->begins[beg_indx] <= self->ends[end_indx]) {
            if (depth_cnt > 0) {
                self->begins[beg_indx] = 0;
                self->unique_cnt--;
            }

            depth_cnt++;
            beg_indx++;
        } else {
            if (depth_cnt > 1) {
                self->ends[end_indx] = 0;
            }

            depth_cnt--;
            end_indx++;
        }
    }

    while (end_indx < self->size - 1) {
        self->ends[end_indx] = 0;
        end_indx++;
    }
}

static int intcmp(const void *lhs, const void *rhs) {
    uint lhs_val = *(const uint *) lhs;
    uint rhs_val = *(const uint *) rhs;

    if (lhs_val > rhs_val) {
        return 1;
    } else if (lhs_val < rhs_val) {
        return -1;
    } else {
        return 0;
    }
}
