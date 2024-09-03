#include "config.h"
#include "config.hash.c"

typedef tlc_config_parser p;
typedef unsigned char uc;

#define error(parser, msg, ...) \
    tlc_error("config: " msg "; line: %lu; pos: %lu; path: %s", \
        __VA_ARGS__ __VA_OPT__(,) \
        (parser)->line_n, \
        (parser)->parsed - (parser)->line_start, \
        (parser)->config->path)

static void error_unexpected_symbol(p *parser, uc c) {
    static const char *state_name[] = { NULL, "key", "=", "value" };
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

static inline int to_bool(const char *key, int len) {
    if (len == 4 && memcmp("true", key, len) == 0) {
        return 1;
    } else if (len == 5 && memcmp("false", key, len) == 0) {
        return 0;
    } else {
        return -1;
    }
}

static inline void ltrim(const char **str, int *len) {
    while (*len > 0) {
        unsigned char c = **str;

        switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                (*str)++;
                (*len)--;
                break;
            default:
                return;
        }
    }
}

static inline void rtrim(const char **str, int *len) {
    while (*len > 0) {
        unsigned char c = (*str)[(*len) - 1];

        switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                (*len)--;
                break;
            default:
                return;
        }
    }
}

static inline void trim(const char **str, int *len) {
    ltrim(str, len);
    rtrim(str, len);
}

static inline void on_val_end(p *parser) {
    const char *val = parser->file->content + parser->word_start;
    int len = parser->parsed - parser->word_start - 1;

    trim(&val, &len);

    if (len == 0) {
        error(parser, "empty value");
    }

    int bool = to_bool(val, len);

    if (bool == -1) {
        error(parser, "unexpected value: %.*s; expected: true or false",
            len, val);
    }

    switch (parser->key) {
        case TLC_CONFIG_KEY_RM_COMMENTS:
            parser->config->rm_comments = bool;
            break;
        case TLC_CONFIG_KEY_ALLOW_SEMICOLON:
            parser->config->allow_semicolon = bool;
            break;
        case TLC_CONFIG_KEY_ALLOW_R:
            parser->config->allow_r = bool;
            break;
        default:
            error(parser, "unknown key id: %d", parser->key);
    }
}

static inline void on_val(p *parser, uc c) {
    if (c == '\n') {
        on_val_end(parser);
        on_newline(parser);
        parser->state = TLC_CONFIG_PARSER_STATE_KEY;
        parser->word_start = parser->parsed;
    }
}

static inline void on_eq(p *parser, uc c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
            return;
        case '\n':
            on_newline(parser);
            return;
        case '=':
            parser->state = TLC_CONFIG_PARSER_STATE_VAL;
            parser->word_start = parser->parsed;
            return;
        default:
            error_unexpected_symbol(parser, c);
    }
}

static inline void on_key_end(p *parser) {
    const char *key = parser->file->content + parser->word_start;
    int len = parser->parsed - parser->word_start - 1;

    if (len == 0) {
        error(parser, "empty key");
    }

    parser->key = tlc_config_key_id(key, len);

    if (!parser->key) {
        error(parser, "unsupported key: %.*s", len, key);
    }

    if (parser->seen_keys[parser->key]) {
        error(parser, "duplicate key: %.*s", len, key);
    }

    parser->seen_keys[parser->key] = 1;
}

static inline void on_key_space(p *parser) {
    if (parser->word_start == parser->parsed - 1) { // leading space
        parser->word_start++;
    } else {
        on_key_end(parser);
        parser->state = TLC_CONFIG_PARSER_STATE_EQ;
    }
}

static inline void on_key(p *parser, uc c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
            on_key_space(parser);
            return;
        case '\n':
            on_key_space(parser);
            on_newline(parser);
            return;
        case '=':
            on_key_end(parser);
            parser->state = TLC_CONFIG_PARSER_STATE_VAL;
            parser->word_start = parser->parsed;
            return;
        case '_':
        TLC_CASE_ALPHA_LOWER:
            return;
        default:
            error_unexpected_symbol(parser, c);
    }
}

static void parse_file(tlc_file *file, tlc_config *config) {
    tlc_config_parser parser = {
        .config = config,
        .file = file,
        .state = TLC_CONFIG_PARSER_STATE_KEY,
        .parsed = 0,
        .line_n = 1,
        .line_start = 0,
        .word_start = 0,
    };

    memset(parser.seen_keys, 0, sizeof(parser.seen_keys));

    while (parser.parsed < file->size) {
        unsigned char c = file->content[parser.parsed++];

        switch (parser.state) {
            case TLC_CONFIG_PARSER_STATE_KEY:
                on_key(&parser, c);
                break;
            case TLC_CONFIG_PARSER_STATE_EQ:
                on_eq(&parser, c);
                break;
            case TLC_CONFIG_PARSER_STATE_VAL:
                on_val(&parser, c);
                break;
            default:
                error(&parser, "unknown parser state: %d", parser.state);
        }
    }

    if (parser.state == TLC_CONFIG_PARSER_STATE_VAL) {
        parser.parsed++; // emulate newline at the end
        on_val_end(&parser);
    } else if (parser.state != TLC_CONFIG_PARSER_STATE_KEY) {
        error(&parser, "unfinished key-value pair");
    }
}

tlc_config *tlc_init_config(const char *path) {
    if (path == NULL && tlc_file_exists(TLC_CONFIG_PATH_DEFAULT)) {
        path = TLC_CONFIG_PATH_DEFAULT;
    }

    tlc_config *config = tlc_malloc_or_error(sizeof(tlc_config));

    config->path = path;
    config->rm_comments = TLC_CONFIG_DEFAULT_RM_COMMENTS;
    config->allow_semicolon = TLC_CONFIG_DEFAULT_ALLOW_SEMICOLON;
    config->allow_r = TLC_CONFIG_DEFAULT_ALLOW_R;

    if (path != NULL) {
        tlc_file *file = tlc_read_file(path);
        parse_file(file, config);
        tlc_free_file(file);
    }

    return config;
}

void tlc_check_config(tlc_config *config) {
    (void)config;
}

void tlc_print_config(tlc_config *config) {
    #define BOOL(value) value ? "true" : "false"

    printf("config:\n");
    printf("    path: %s\n", config->path);
    printf("    rm_comments: %s\n", BOOL(config->rm_comments));
    printf("    allow_semicolon: %s\n", BOOL(config->allow_semicolon));
    printf("    allow_r: %s\n", BOOL(config->allow_r));

    #undef BOOL

    if (config->path) {
        tlc_file *file = tlc_read_file(config->path);
        tlc_print_file(file, "config file");
        tlc_free_file(file);
    }
}

void tlc_free_config(tlc_config *config) {
    free(config);
}
