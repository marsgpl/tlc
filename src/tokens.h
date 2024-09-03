#ifndef TLC_TOKENS_H
#define TLC_TOKENS_H

#include "error.h"

#define TLC_TOKENS_DEFAULT_CAPACITY 512

typedef struct {
    int type;
    size_t pos;
    size_t len;
} tlc_token;

typedef struct {
    int capacity;
    int count;
    tlc_token *list;
} tlc_tokens;

tlc_tokens *tlc_init_tokens(int capacity);
void tlc_print_tokens(tlc_tokens *tokens);
void tlc_free_tokens(tlc_tokens *tokens);

#endif
