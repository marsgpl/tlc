#include "parser.h"

typedef tlc_parser p;
typedef unsigned char uc;
typedef tlc_token_type tt;

static void on_token_start(p *parser, uc c);

#define error(parser, msg, ...) \
    tlc_error(msg "; line: %lu; pos: %lu; path: %s", \
        __VA_ARGS__ __VA_OPT__(,) \
        (parser)->line_n, \
        (parser)->parsed - (parser)->line_start, \
        (parser)->file->path)

static void error_unexpected_symbol(p *parser, uc c) {
    static const char *state_name[] = { NULL, "block start" };
    const char *expected = state_name[parser->state];

    if (isprint(c)) {
        error(parser, "unexpected symbol: %c; expected: %s", c, expected);
    } else {
        error(parser, "unexpected symbol: 0x%X%X; expected: %s",
            (c >> 4) % 16,
            c % 16,
            expected);
    }
}

static inline void on_newline(p *parser) {
    parser->line_n++;
    parser->line_start = parser->parsed;
}

static inline void on_word_end(p *parser) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    const char *word = parser->file->content + parser->word_start;
    int len = parser->parsed - parser->word_start - 1;
    int kw_id = tlc_keyword_id(word, len);

    tlc_token *token = tlc_push_token(parser->tokens);

    token->pos = parser->word_start;
    token->len = len;

    if (kw_id) {
        token->type = TLC_TOKEN_KEYWORD;
        token->id = kw_id;
    } else {
        token->type = TLC_TOKEN_WORD;
    }
}

static inline void on_word(p *parser, uc c) {
    switch (c) {
        TLC_CASE_ALPHA_LOWER:
        TLC_CASE_ALPHA_UPPER:
        TLC_CASE_DIGIT:
        case '_':
            return;
        default:
            on_word_end(parser);
            on_token_start(parser, c);
            return;
    }
}

static inline void on_duo(p *parser, tt small, uc next, tt big) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->pos = parser->parsed - 1;

    if (parser->parsed < parser->file->size
        && parser->file->content[parser->parsed] == next
    ) {
        token->type = big;
        token->len = 2;
        parser->parsed++;
    } else {
        token->type = small;
        token->len = 1;
    }
}

static inline void on_single(p *parser, tt type) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->type = type;
    token->pos = parser->parsed - 1;
    token->len = 1;
}

static inline void on_hyphen(p *parser) {
    if (parser->parsed < parser->file->size
        && parser->file->content[parser->parsed] == '-'
    ) {
        if (parser->parsed + 2 < parser->file->size
            && parser->file->content[parser->parsed + 1] == '['
            && parser->file->content[parser->parsed + 2] == '['
        ) {
            parser->word_start = parser->parsed - 1;
            parser->state = TLC_PARSER_STATE_MULTI_LINE_COMMENT;
            parser->parsed += 3;
        } else {
            parser->word_start = parser->parsed - 1;
            parser->state = TLC_PARSER_STATE_SINGLE_LINE_COMMENT;
            parser->parsed++;
        }
    } else {
        on_single(parser, TLC_TOKEN_MINUS);
    }
}

static inline void on_left_angle(p *parser) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->pos = parser->parsed - 1;

    if (parser->parsed < parser->file->size) {
        unsigned char next = parser->file->content[parser->parsed];

        switch (next) {
            case '<':
                token->type = TLC_TOKEN_L_SHIFT;
                token->len = 2;
                return;
            case '=':
                token->type = TLC_TOKEN_LTE;
                token->len = 2;
                return;
        }
    }

    token->type = TLC_TOKEN_L_ANGLE_BR;
    token->len = 1;
}

static inline void on_right_angle(p *parser) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->pos = parser->parsed - 1;

    if (parser->parsed < parser->file->size) {
        unsigned char next = parser->file->content[parser->parsed];

        switch (next) {
            case '>':
                token->type = TLC_TOKEN_R_SHIFT;
                token->len = 2;
                return;
            case '=':
                token->type = TLC_TOKEN_GTE;
                token->len = 2;
                return;
        }
    }

    token->type = TLC_TOKEN_R_ANGLE_BR;
    token->len = 1;
}

static void on_token_start(p *parser, uc c) {
    switch (c) {
        TLC_CASE_ALPHA_LOWER:
        TLC_CASE_ALPHA_UPPER:
        case '_':
            parser->word_start = parser->parsed - 1;
            parser->state = TLC_PARSER_STATE_WORD;
            return;
        TLC_CASE_DIGIT:
            parser->word_start = parser->parsed - 1;
            parser->state = TLC_PARSER_STATE_NUMBER;
            return;
        case '\'':
            parser->word_start = parser->parsed - 1;
            parser->state = TLC_PARSER_STATE_SINGLE_QUOTE_STRING;
            return;
        case '"':
            parser->word_start = parser->parsed - 1;
            parser->state = TLC_PARSER_STATE_DOUBLE_QUOTE_STRING;
            return;
        case ' ':
        case '\t':
        case '\r':
            return; // skip
        case '\n': on_newline(parser); return;
        case '-': on_hyphen(parser); return;
        case '<': on_left_angle(parser); return;
        case '>': on_right_angle(parser); return;
        case '=': on_duo(parser, TLC_TOKEN_ASSIGN, '=', TLC_TOKEN_EQ); return;
        case '/': on_duo(parser, TLC_TOKEN_SLASH, '/', TLC_TOKEN_SS); return;
        case ':': on_duo(parser, TLC_TOKEN_COLON, ':', TLC_TOKEN_CC); return;
        case '~': on_duo(parser, TLC_TOKEN_TILDE, '=', TLC_TOKEN_NEQ); return;
        case ';': on_single(parser, TLC_TOKEN_SEMICOLON); return;
        case '*': on_single(parser, TLC_TOKEN_ASTERISK); return;
        case '%': on_single(parser, TLC_TOKEN_PERCENT); return;
        case '^': on_single(parser, TLC_TOKEN_CARET); return;
        case ',': on_single(parser, TLC_TOKEN_COMMA); return;
        case '+': on_single(parser, TLC_TOKEN_PLUS); return;
        case '#': on_single(parser, TLC_TOKEN_HASH); return;
        case '&': on_single(parser, TLC_TOKEN_AMP); return;
        case '|': on_single(parser, TLC_TOKEN_BAR); return;
        case '(': on_single(parser, TLC_TOKEN_L_ROUND_BR); return;
        case ')': on_single(parser, TLC_TOKEN_R_ROUND_BR); return;
        case '{': on_single(parser, TLC_TOKEN_L_CURL_BR); return;
        case '}': on_single(parser, TLC_TOKEN_R_CURL_BR); return;
        case ']': on_single(parser, TLC_TOKEN_R_SQ_BR); return;
        default:
            error_unexpected_symbol(parser, c);
/*
    TLC_TOKEN_FLOAT,

    TLC_TOKEN_L_SQ_BR, // [
    TLC_TOKEN_MULTI_LINE_STRING, // [[

    TLC_TOKEN_PERIOD, // .
    TLC_TOKEN_CONCAT, // ..
    TLC_TOKEN_ELLIPSIS, // ...
*/
    }
}

static inline void on_number_end(p *parser) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->type = TLC_TOKEN_INTEGER;
    token->pos = parser->word_start;
    token->len = parser->parsed - parser->word_start - 1;
}

static inline void on_string_end(p *parser, tt type) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->type = type;
    token->pos = parser->word_start;
    token->len = parser->parsed - parser->word_start;
}

static inline void on_single_line_comment_end(p *parser) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->type = TLC_TOKEN_SINGLE_LINE_COMMENT;
    token->pos = parser->word_start;
    token->len = parser->parsed - parser->word_start - 1;
}

static inline void on_multi_line_comment_end(p *parser) {
    parser->state = TLC_PARSER_STATE_TOKEN_START;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->type = TLC_TOKEN_MULTI_LINE_COMMENT;
    token->pos = parser->word_start;
    token->len = parser->parsed - parser->word_start + 1;
}

static inline void on_number(p *parser, uc c) {
    switch (c) {
        TLC_CASE_DIGIT:
        case 'x':
        case 'X':
        TLC_CASE_HEX_LOWER:
        TLC_CASE_HEX_UPPER:
            return;
// TLC_TOKEN_FLOAT: . eE pP -+
        default:
            on_number_end(parser);
            on_token_start(parser, c);
            return;
    }
}

static inline void on_string_escape(p *parser) {
    if (parser->parsed >= parser->file->size) {
        error(parser, "unfinished string escape sequence");
    }

    parser->parsed++;
}

static inline void on_string_single_quote(p *parser, uc c) {
    switch (c) {
        case '\'':
            on_string_end(parser, TLC_TOKEN_SINGLE_QUOTE_STRING);
            return;
        case '\\':
            on_string_escape(parser);
            return;
        case '\n':
            error(parser, "single quote string can't have line break");
        default:
            return;
    }
}

static inline void on_string_double_quote(p *parser, uc c) {
    switch (c) {
        case '"':
            on_string_end(parser, TLC_TOKEN_DOUBLE_QUOTE_STRING);
            return;
        case '\\':
            on_string_escape(parser);
            return;
        case '\n':
            error(parser, "double quote string can't have line break");
        default:
            return;
    }
}

static inline void on_single_line_comment(p *parser, uc c) {
    if (c == '\n') {
        on_single_line_comment_end(parser);
        on_newline(parser);
    }
}

static inline void on_multi_line_comment(p *parser, uc c) {
    if (c == ']'
        && parser->parsed < parser->file->size
        && parser->file->content[parser->parsed] == ']'
    ) {
        on_multi_line_comment_end(parser);
        parser->parsed++;
    }
}

static inline void on_c(p *parser, uc c) {
    switch (parser->state) {
        case TLC_PARSER_STATE_TOKEN_START:
            on_token_start(parser, c);
            return;
        case TLC_PARSER_STATE_WORD:
            on_word(parser, c);
            return;
        case TLC_PARSER_STATE_NUMBER:
            on_number(parser, c);
            return;
        case TLC_PARSER_STATE_SINGLE_QUOTE_STRING:
            on_string_single_quote(parser, c);
            return;
        case TLC_PARSER_STATE_DOUBLE_QUOTE_STRING:
            on_string_double_quote(parser, c);
            return;
        case TLC_PARSER_STATE_SINGLE_LINE_COMMENT:
            on_single_line_comment(parser, c);
            return;
        case TLC_PARSER_STATE_MULTI_LINE_COMMENT:
            on_multi_line_comment(parser, c);
            return;
        default:
            error(parser, "unknown parser state: %d", parser->state);
    }
}

tlc_tokens *tlc_parse_file(tlc_file *file, tlc_config *config) {
    tlc_tokens *tokens = tlc_init_tokens(0);

    tlc_parser parser = {
        .config = config,
        .tokens = tokens,
        .file = file,
        .state = TLC_PARSER_STATE_TOKEN_START,
        .parsed = 0,
        .line_n = 1,
        .line_start = 0,
    };

    while (parser.parsed < file->size) {
        unsigned char c = file->content[parser.parsed++];
        on_c(&parser, c);
    }

    if (parser.state != TLC_PARSER_STATE_TOKEN_START) {
        parser.parsed++;
        on_c(&parser, ' ');
    }

    if (parser.state != TLC_PARSER_STATE_TOKEN_START) {
        parser.parsed--;
        error(&parser, "unfinished token");
    }

    return tokens;
}
