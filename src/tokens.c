#include "tokens.h"
#include "tokens.hash.c"

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

void tlc_print_tokens(tlc_tokens *tokens, tlc_file *file) {
    printf("tokens:\n");
    printf("    capacity: %d\n", tokens->capacity);
    printf("    count: %d\n", tokens->count);

    for (int i = 0; i < tokens->count; ++i) {
        tlc_token token = tokens->list[i];
        const char *word = &(file->content[token.pos]);

        printf("        %.*s\n", token.len, word);
    }
}

void tlc_free_tokens(tlc_tokens *tokens) {
    free(tokens->list);
    free(tokens);
}

tlc_token *tlc_push_token(tlc_tokens *tokens) {
    if (tokens->count >= tokens->capacity) {
        tokens->capacity += TLC_TOKENS_DEFAULT_CAPACITY;
        tokens->list = tlc_realloc_or_error(tokens->list,
            sizeof(tlc_token) * tokens->capacity);
    }

    return &(tokens->list[tokens->count++]);
}
