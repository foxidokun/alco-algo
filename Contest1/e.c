#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------------------------------------------------
// Const & Define
// ---------------------------------------------------------------------------------------------------------------------

const int START_CAPACITY = 1024;

// ---------------------------------------------------------------------------------------------------------------------
// Struct Definition
// ---------------------------------------------------------------------------------------------------------------------

struct speech {
    char *symbols;
    size_t capacity;
    size_t size;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void speech_init(struct speech *speech);
static void speech_free(struct speech *speech);
static int speech_insert(struct speech *speech, char symbol);
static int fold_last_symbol(struct speech *speech, char symbol);

// ---------------------------------------------------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------------------------------------------------

int main (void) {
    int ch = 0;

    struct speech speech;
    speech_init (&speech);

    int still_valid = 1;

    while ((ch = getchar()) != EOF) {
        still_valid = still_valid && speech_insert (&speech, (char) ch);

        if (!still_valid) {
            printf ("NO\n");
            speech_free (&speech);
            return 0;
        }
    }

    if (speech.size == 0) {
        printf("YES\n");
    } else {
        printf("NO\n");
    }

    speech_free(&speech);
    return 0;
}

static void speech_init(struct speech *speech) {
    speech->symbols = (char *) calloc (START_CAPACITY, sizeof (char));
    speech->size = 0;
    speech->capacity = START_CAPACITY;
}

static void speech_free(struct speech *speech) {
    free (speech->symbols);
}


static int speech_insert(struct speech *speech, char symbol) {
    if (speech->size == speech->capacity) {
        speech->symbols = (char *) realloc (speech->symbols, speech->capacity * 2 * sizeof (int));
        speech->capacity = speech->capacity << 1;
    }

    switch (symbol) {
        case '(':
        case '{':
        case '[':
            speech->symbols[speech->size] = symbol;
            speech->size++;
            return 1;

        case ')': return fold_last_symbol(speech, '(');
        case '}': return fold_last_symbol(speech, '{');
        case ']': return fold_last_symbol(speech, '[');


        default:
            return -1;
    }
}

static int fold_last_symbol(struct speech *speech, char symbol) {
    if (speech->size == 0) {
        return 0;
    }

    if (speech->symbols[--speech->size] == symbol) {
        return 1;
    } else {
        return 0;
    }
}
