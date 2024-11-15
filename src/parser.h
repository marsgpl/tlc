#ifndef TLC_PARSER_H
#define TLC_PARSER_H

#include "error.h"
#include "file.h"
#include "config.h"
#include "tokens.h"
#include "case.h"

typedef enum {
    TLC_PARSER_STATE_TOKEN = 1,
    TLC_PARSER_STATE_WORD,
    TLC_PARSER_STATE_SINGLE_QUOTE_STRING,
    TLC_PARSER_STATE_DOUBLE_QUOTE_STRING,
    TLC_PARSER_STATE_MULTI_LINE_STRING,
    TLC_PARSER_STATE_LONG_BRACKET_START,
    TLC_PARSER_STATE_LONG_BRACKET,
    TLC_PARSER_STATE_SINGLE_LINE_COMMENT,
    TLC_PARSER_STATE_MULTI_LINE_COMMENT,
} tlc_parser_state;

typedef enum {
    TLC_PARSER_NUM_0 = 1, // 0
    TLC_PARSER_NUM_DIG, // 1
    TLC_PARSER_NUM_DOT, // .2  3.
    TLC_PARSER_NUM_X, // 0x
    TLC_PARSER_NUM_E,
    TLC_PARSER_NUM_X_DIG,
    TLC_PARSER_NUM_X_DOT, // 0x.
    TLC_PARSER_NUM_X_DIG_DOT, // 0x1.
    TLC_PARSER_NUM_X_DOT_DIG, // 0x.1
    TLC_PARSER_NUM_E_DIG,
    TLC_PARSER_NUM_E_SIGN,
} tlc_parser_num;

typedef struct {
    tlc_config *config;
    tlc_tokens *tokens;
    tlc_file *file;
    tlc_parser_state state;
    size_t parsed;
    size_t line_n;
    size_t line_start;
    size_t word_start;
    int bracket_level;
} tlc_parser;

tlc_tokens *tlc_parse_file(tlc_file *file, tlc_config *config);

#endif
