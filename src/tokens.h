#ifndef TLC_TOKENS_H
#define TLC_TOKENS_H

#include "error.h"
#include "file.h"

#define TLC_TOKENS_DEFAULT_CAPACITY 1024

#define TLC_KEYWORDS_N 22

typedef enum {
    TLC_KW_AND = 1,
    TLC_KW_BREAK,
    TLC_KW_DO,
    TLC_KW_ELSE,
    TLC_KW_ELSEIF,
    TLC_KW_END,
    TLC_KW_FALSE,
    TLC_KW_FOR,
    TLC_KW_FUNCTION,
    TLC_KW_GOTO,
    TLC_KW_IF,
    TLC_KW_IN,
    TLC_KW_LOCAL,
    TLC_KW_NIL,
    TLC_KW_NOT,
    TLC_KW_OR,
    TLC_KW_REPEAT,
    TLC_KW_RETURN,
    TLC_KW_THEN,
    TLC_KW_TRUE,
    TLC_KW_UNTIL,
    TLC_KW_WHILE,
} tlc_keyword;

typedef enum {
    TLC_TOKEN_KEYWORD = 1,
    TLC_TOKEN_WORD,
    TLC_TOKEN_INTEGER,
    TLC_TOKEN_FLOAT,
    TLC_TOKEN_SINGLE_QUOTE_STRING, // 'xxx'
    TLC_TOKEN_DOUBLE_QUOTE_STRING, // "xxx"
    TLC_TOKEN_SEMICOLON, // ;
    TLC_TOKEN_ASTERISK, // *
    TLC_TOKEN_PERCENT, // %
    TLC_TOKEN_CARET, // ^
    TLC_TOKEN_COMMA, // ,
    TLC_TOKEN_PLUS, // +
    TLC_TOKEN_HASH, // #
    TLC_TOKEN_AMP, // &
    TLC_TOKEN_BAR, // |
    TLC_TOKEN_L_ROUND_BR, // (
    TLC_TOKEN_R_ROUND_BR, // )
    TLC_TOKEN_L_CURL_BR, // {
    TLC_TOKEN_R_CURL_BR, // }
    TLC_TOKEN_R_SQ_BR, // ]
    TLC_TOKEN_L_SQ_BR, // [
    TLC_TOKEN_MULTI_LINE_STRING, // [[
    TLC_TOKEN_ASSIGN, // =
    TLC_TOKEN_EQ, // ==
    TLC_TOKEN_SLASH, // /
    TLC_TOKEN_SS, // //
    TLC_TOKEN_COLON, // :
    TLC_TOKEN_CC, // ::
    TLC_TOKEN_TILDE, // ~
    TLC_TOKEN_NEQ, // ~=
    TLC_TOKEN_L_ANGLE_BR, // <
    TLC_TOKEN_L_SHIFT, // <<
    TLC_TOKEN_LTE, // <=
    TLC_TOKEN_R_ANGLE_BR, // >
    TLC_TOKEN_R_SHIFT, // >>
    TLC_TOKEN_GTE, // >=
    TLC_TOKEN_PERIOD, // .
    TLC_TOKEN_CONCAT, // ..
    TLC_TOKEN_ELLIPSIS, // ...
    TLC_TOKEN_MINUS, // -
    TLC_TOKEN_SINGLE_LINE_COMMENT, // --
    TLC_TOKEN_MULTI_LINE_COMMENT, // --[[
} tlc_token_type;

typedef struct {
    tlc_token_type type;
    int id;
    size_t pos;
    int len;
} tlc_token;

typedef struct {
    int capacity;
    int count;
    tlc_token *list;
} tlc_tokens;

tlc_tokens *tlc_init_tokens(int capacity);
void tlc_print_tokens(tlc_tokens *tokens, tlc_file *file);
void tlc_free_tokens(tlc_tokens *tokens);
int tlc_keyword_id(const char *str, int len);
tlc_token *tlc_push_token(tlc_tokens *tokens);

#endif
