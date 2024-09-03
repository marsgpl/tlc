#include "tokens.h"

tlc_tokens *tlc_init_tokens(int capacity) {
    tlc_tokens *tokens = tlc_malloc_or_error(sizeof(tlc_tokens));

    if (capacity <= 0) {
        capacity = TLC_TOKENS_DEFAULT_CAPACITY;
    }

    tokens->capacity = capacity;
    tokens->count = 0;
    tokens->list = tlc_malloc_or_error(sizeof(tlc_token) * tokens->capacity);

    return tokens;
}

void tlc_print_tokens(tlc_tokens *tokens) {
    printf("tokens:\n");
    printf("    capacity: %d\n", tokens->capacity);
    printf("    count: %d\n", tokens->count);
}

void tlc_free_tokens(tlc_tokens *tokens) {
    free(tokens->list);
    free(tokens);
}
