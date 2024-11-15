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
    static const char *state_name[] = {
        NULL,
        "token",
        "word",
        "NUM_0",
        "NUM_DIG",
        "NUM_DOT",
        "NUM_X",
        "NUM_E",
        "NUM_X_DIG",
        "NUM_X_DOT",
        "NUM_E_DIG",
        "NUM_E_SIGN",
        "single quote string",
        "double quote string",
        "multi line string",
        "long bracket start",
        "long bracket",
        "single line comment",
        "multi line comment",
    };
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

static inline void on_composite(p *parser, tlc_parser_state state) {
    parser->state = state;
    parser->word_start = parser->parsed - 1;
}

static inline tlc_token *on_token(p *parser, tt type, size_t pos, int len) {
    parser->state = TLC_PARSER_STATE_TOKEN;

    tlc_token *token = tlc_push_token(parser->tokens);

    token->type = type;
    token->pos = pos;
    token->len = len;

    return token;
}

static inline void on_word_end(p *parser) {
    size_t pos = parser->word_start;
    int len = parser->parsed - pos - 1;

    const char *word = parser->file->content + pos;
    int kw_id = tlc_keyword_id(word, len);

    if (kw_id) {
        tlc_token *token = on_token(parser, TLC_TOKEN_KEYWORD, pos, len);
        token->id = kw_id;
    } else {
        on_token(parser, TLC_TOKEN_WORD, pos, len);
    }
}

static inline void on_string_end(p *parser, tt type) {
    size_t pos = parser->word_start;
    int len = parser->parsed - pos; // include quote

    on_token(parser, type, pos, len);
}

static inline void on_single_line_comment_end(p *parser) {
    size_t pos = parser->word_start;
    int len = parser->parsed - pos - 1;

    on_token(parser, TLC_TOKEN_SINGLE_LINE_COMMENT, pos, len);
}

static inline void on_multi_line_comment_end(p *parser) {
    size_t pos = parser->word_start;
    int len = parser->parsed - pos + 1; // include ]]

    on_token(parser, TLC_TOKEN_MULTI_LINE_COMMENT, pos, len);
}

static inline void on_multi_line_string_end(p *parser) {
    size_t pos = parser->word_start;
    int len = parser->parsed - pos + 1; // include ]]

    on_token(parser, TLC_TOKEN_MULTI_LINE_STRING, pos, len);
}

static inline void on_long_bracket_end(p *parser) {
    size_t pos = parser->word_start;
    int len = parser->parsed - pos + parser->bracket_level + 1; // ]===]

    on_token(parser, TLC_TOKEN_LONG_BRACKET_STRING, pos, len);
}

static inline void on_1(p *parser, tt type) {
    on_token(parser, type, parser->parsed - 1, 1);
}

static inline void on_2(p *parser, tt type) {
    on_token(parser, type, parser->parsed - 1, 2);
    parser->parsed++;
}

static inline void on_3(p *parser, tt type) {
    on_token(parser, type, parser->parsed - 1, 3);
    parser->parsed += 2;
}

static inline void on_1_2(p *parser, tt small, uc next, tt big) {
    if (parser->parsed < parser->file->size &&
        parser->file->content[parser->parsed] == next
    ) {
        on_2(parser, big);
    } else {
        on_1(parser, small);
    }
}

static inline void on_hyphen(p *parser) {
    if (parser->parsed < parser->file->size &&
        parser->file->content[parser->parsed] == '-'
    ) {
        if (parser->parsed + 2 < parser->file->size &&
            parser->file->content[parser->parsed + 1] == '[' &&
            parser->file->content[parser->parsed + 2] == '['
        ) {
            on_composite(parser, TLC_PARSER_STATE_MULTI_LINE_COMMENT);
            parser->parsed += 3;
        } else {
            on_composite(parser, TLC_PARSER_STATE_SINGLE_LINE_COMMENT);
            parser->parsed++;
        }
    } else {
        on_1(parser, TLC_TOKEN_MINUS);
    }
}

static inline void on_left_angle(p *parser) {
    if (parser->parsed < parser->file->size) {
        switch (parser->file->content[parser->parsed]) {
            case '<': on_2(parser, TLC_TOKEN_L_SHIFT); return;
            case '=': on_2(parser, TLC_TOKEN_LTE); return;
        }
    }

    on_1(parser, TLC_TOKEN_L_ANGLE_BR);
}

static inline void on_right_angle(p *parser) {
    if (parser->parsed < parser->file->size) {
        switch (parser->file->content[parser->parsed]) {
            case '>': on_2(parser, TLC_TOKEN_R_SHIFT); return;
            case '=': on_2(parser, TLC_TOKEN_GTE); return;
        }
    }

    on_1(parser, TLC_TOKEN_R_ANGLE_BR);
}

static inline void on_sq_open(p *parser) {
    if (parser->parsed < parser->file->size) {
        switch (parser->file->content[parser->parsed]) {
            case '[':
                on_composite(parser, TLC_PARSER_STATE_MULTI_LINE_STRING);
                parser->parsed++;
                return;
            case '=':
                on_composite(parser, TLC_PARSER_STATE_LONG_BRACKET_START);
                parser->parsed++;
                return;
        }
    }

    on_1(parser, TLC_TOKEN_L_SQ_BR);
}

static inline void on_number_end(p *parser, tt type) {
    size_t pos = parser->word_start;
    int len = parser->parsed - pos - 1;

    on_token(parser, type, pos, len);
}

static inline void parse_number(
    p *parser,
    tlc_parser_num state,
    int len,
    tlc_token_type type
) {
    parser->word_start = parser->parsed - len;
    int is_hex = 0;
state
type
}

static inline void on_dot(p *parser) {
    if (parser->parsed < parser->file->size) {
        switch (parser->file->content[parser->parsed]) {
            TLC_CASE_DIGIT:
                parser->parsed++;
                parse_number(parser, TLC_PARSER_NUM_DOT, 2, TLC_TOKEN_FLOAT);
                return;
            case '.':
                if (parser->parsed + 1 < parser->file->size &&
                    parser->file->content[parser->parsed + 1] == '.'
                ) {
                    on_3(parser, TLC_TOKEN_ELLIPSIS);
                    return;
                }

                on_2(parser, TLC_TOKEN_CONCAT);
                return;
        }
    }

    on_1(parser, TLC_TOKEN_DOT);
}

static void on_token_start(p *parser, uc c) {
    switch (c) {
        TLC_CASE_ALPHA_LOWER:
        TLC_CASE_ALPHA_UPPER:
        case '_':
            on_composite(parser, TLC_PARSER_STATE_WORD);
            return;
        case '0':
            parse_number(parser, TLC_PARSER_NUM_0, 1, TLC_TOKEN_INTEGER);
            return;
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
            parse_number(parser, TLC_PARSER_NUM_DIG, 1, TLC_TOKEN_INTEGER);
            return;
        case '\'':
            on_composite(parser, TLC_PARSER_STATE_SINGLE_QUOTE_STRING);
            return;
        case '"':
            on_composite(parser, TLC_PARSER_STATE_DOUBLE_QUOTE_STRING);
            return;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return;
        case '.': on_dot(parser); return;
        case '-': on_hyphen(parser); return;
        case '<': on_left_angle(parser); return;
        case '>': on_right_angle(parser); return;
        case '[': on_sq_open(parser); return;
        case '=': on_1_2(parser, TLC_TOKEN_ASSIGN, '=', TLC_TOKEN_EQ); return;
        case '/': on_1_2(parser, TLC_TOKEN_SLASH, '/', TLC_TOKEN_SS); return;
        case ':': on_1_2(parser, TLC_TOKEN_COLON, ':', TLC_TOKEN_CC); return;
        case '~': on_1_2(parser, TLC_TOKEN_TILDE, '=', TLC_TOKEN_NEQ); return;
        case ';': on_1(parser, TLC_TOKEN_SEMICOLON); return;
        case '*': on_1(parser, TLC_TOKEN_ASTERISK); return;
        case '%': on_1(parser, TLC_TOKEN_PERCENT); return;
        case '^': on_1(parser, TLC_TOKEN_CARET); return;
        case ',': on_1(parser, TLC_TOKEN_COMMA); return;
        case '+': on_1(parser, TLC_TOKEN_PLUS); return;
        case '#': on_1(parser, TLC_TOKEN_HASH); return;
        case '&': on_1(parser, TLC_TOKEN_AMP); return;
        case '|': on_1(parser, TLC_TOKEN_BAR); return;
        case '(': on_1(parser, TLC_TOKEN_L_ROUND_BR); return;
        case ')': on_1(parser, TLC_TOKEN_R_ROUND_BR); return;
        case '{': on_1(parser, TLC_TOKEN_L_CURL_BR); return;
        case '}': on_1(parser, TLC_TOKEN_R_CURL_BR); return;
        case ']': on_1(parser, TLC_TOKEN_R_SQ_BR); return;
        default:
            error_unexpected_symbol(parser, c);
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
            error(parser, "single quote string can't have line breaks");
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
            error(parser, "double quote string can't have line breaks");
        default:
            return;
    }
}

static inline void on_single_line_comment(p *parser, uc c) {
    if (c == '\n') {
        on_single_line_comment_end(parser);
    }
}

static inline void on_multi_line_comment(p *parser, uc c) {
    if (c == ']' &&
        parser->parsed < parser->file->size &&
        parser->file->content[parser->parsed] == ']'
    ) {
        on_multi_line_comment_end(parser);
        parser->parsed++;
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

static inline void on_multi_line_string(p *parser, uc c) {
    if (c == ']' &&
        parser->parsed < parser->file->size &&
        parser->file->content[parser->parsed] == ']'
    ) {
        on_multi_line_string_end(parser);
        parser->parsed++;
    }
}

static inline void on_long_bracket(p *parser, uc c) {
    if (c == ']' &&
        parser->parsed + parser->bracket_level < parser->file->size
    ) {
        on_long_bracket_end(parser);
        parser->parsed += parser->bracket_level + 1;
    }
}

static inline void on_long_bracket_start(p *parser, uc c) {
    switch (c) {
        case '=':
            return;
        case '[':
            parser->bracket_level = parser->parsed - parser->word_start - 2;
            parser->state = TLC_PARSER_STATE_LONG_BRACKET;
            return;
        default:
            error_unexpected_symbol(parser, c);
    }
}

static inline void on_c(p *parser, uc c) {
    switch (parser->state) {
        case TLC_PARSER_STATE_TOKEN:
            on_token_start(parser, c);
            break;
        case TLC_PARSER_STATE_WORD:
            on_word(parser, c);
            break;
        case TLC_PARSER_STATE_SINGLE_QUOTE_STRING:
            on_string_single_quote(parser, c);
            break;
        case TLC_PARSER_STATE_DOUBLE_QUOTE_STRING:
            on_string_double_quote(parser, c);
            break;
        case TLC_PARSER_STATE_MULTI_LINE_STRING:
            on_multi_line_string(parser, c);
            break;
        case TLC_PARSER_STATE_LONG_BRACKET_START:
            on_long_bracket_start(parser, c);
            break;
        case TLC_PARSER_STATE_LONG_BRACKET:
            on_long_bracket(parser, c);
            break;
        case TLC_PARSER_STATE_SINGLE_LINE_COMMENT:
            on_single_line_comment(parser, c);
            break;
        case TLC_PARSER_STATE_MULTI_LINE_COMMENT:
            on_multi_line_comment(parser, c);
            break;
        default:
            error(parser, "unknown parser state: %d", parser->state);
    }

    if (c == '\n') {
        parser->line_n++;
        parser->line_start = parser->parsed;
    }
}

tlc_tokens *tlc_parse_file(tlc_file *file, tlc_config *config) {
    tlc_tokens *tokens = tlc_init_tokens(0);

    tlc_parser parser = {
        .config = config,
        .tokens = tokens,
        .file = file,
        .state = TLC_PARSER_STATE_TOKEN,
        .parsed = 0,
        .line_n = 1,
        .line_start = 0,
    };

    while (parser.parsed < file->size) {
        unsigned char c = file->content[parser.parsed++];
        on_c(&parser, c);
    }

    switch (parser.state) {
        case TLC_PARSER_STATE_TOKEN:
            break;
        case TLC_PARSER_STATE_WORD:
            parser.parsed++; // emulate space
            on_word_end(&parser);
            break;
        case TLC_PARSER_STATE_SINGLE_LINE_COMMENT:
            parser.parsed++; // emulate \n
            on_single_line_comment_end(&parser);
            break;
        default:
            error(&parser, "unfinished token");
    }

    return tokens;
}
