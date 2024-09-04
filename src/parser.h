#ifndef TLC_PARSER_H
#define TLC_PARSER_H

#include "error.h"
#include "file.h"
#include "config.h"
#include "tokens.h"
#include "case.h"

typedef enum {
    TLC_PARSER_STATE_TOKEN_START = 1,
    TLC_PARSER_STATE_WORD,
    TLC_PARSER_STATE_NUMBER,
    TLC_PARSER_STATE_SINGLE_QUOTE_STRING,
    TLC_PARSER_STATE_DOUBLE_QUOTE_STRING,
    TLC_PARSER_STATE_SINGLE_LINE_COMMENT,
    TLC_PARSER_STATE_MULTI_LINE_COMMENT,
} tlc_parser_state;

typedef struct {
    tlc_config *config;
    tlc_tokens *tokens;
    tlc_file *file;
    tlc_parser_state state;
    size_t parsed;
    size_t line_n;
    size_t line_start;
    size_t word_start;
} tlc_parser;

tlc_tokens *tlc_parse_file(tlc_file *file, tlc_config *config);

#endif
