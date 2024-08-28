#include "config.h"
#include "config.hash.c"

typedef tlc_config_parser p;
typedef unsigned char uc;

#define tlc_config_error(parser, msg, ...) \
    tlc_error("config: " msg "; line: %d; pos: %d; path: %s", \
        __VA_ARGS__, \
        (parser)->line_n, \
        (parser)->pos_n, \
        (parser)->config->path)

static void tlc_config_unexpected_symbol(p *parser, uc c) {
    static const char *state_name[] = { NULL, "key or =", "=", "value" };
    const char *expected = state_name[parser->state];

    if (isprint(c)) {
        tlc_config_error(parser, "unexpected symbol: %c; expected: %s",
            c, expected);
    } else {
        tlc_config_error(parser, "unexpected symbol: 0x%X%X; expected: %s",
            (c >> 4) % 16,
            c % 16,
            expected);
    }
}

static inline void tlc_parse_config_on_newline(p *parser) {
    parser->pos_n = 0; // auto incr before iter, so effectively starts at 1
    parser->line_n++;
}

static inline void tlc_parse_config_key_on_end(p *parser) {
    const char *key = parser->file->content + parser->word_start - 1;
    int len = parser->parsed - parser->word_start;

    if (len == 0) {
        tlc_config_error(parser, "%s", "empty key");
    }

    parser->key = tlc_config_key_id(key, len);

    if (!parser->key) {
        tlc_config_error(parser, "unknown key: %.*s", len, key);
    }
}

static inline void tlc_parse_config_key_on_space(p *parser) {
    if (parser->word_start == parser->parsed) {
        parser->word_start++; // skip leading space
    } else {
        tlc_parse_config_key_on_end(parser);
        parser->state = TLC_CONFIG_PARSER_STATE_EQ;
    }
}

static inline void tlc_parse_config_val_on_space(p *parser) {
    if (parser->word_start == parser->parsed) {
        parser->word_start++; // skip leading space
    }

    // non-leading spaces are allowed
    // trailing spaces are removed by tlc_parse_config_val_on_end
}

static inline int tlc_rtrim(const char *str, int len) {
    while (len > 0) {
        unsigned char c = str[len - 1];

        if (isprint(c)) {
            return len;
        }

        len--;
    }

    return len;
}

static inline int tlc_config_to_bool(const char *key, int len) {
    if (len == 4 && memcmp("true", key, len) == 0) {
        return 1;
    } else if (len == 5 && memcmp("false", key, len) == 0) {
        return 0;
    } else {
        return -1;
    }
}

static inline void tlc_parse_config_val_on_end(p *parser) {
    const char *val = parser->file->content + parser->word_start - 1;
    int len = tlc_rtrim(val, parser->parsed - parser->word_start);
    int bool = tlc_config_to_bool(val, len);

    if (len == 0) {
        tlc_config_error(parser, "%s", "empty value");
    }

    if (bool == -1) {
        tlc_config_error(parser,
            "unexpected value: %.*s; expected: true or false",
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
            tlc_config_error(parser, "unknown key: %d", parser->key);
    }
}

static inline void tlc_parse_config_val_on_r(p *parser) {
    if (parser->parsed < parser->file->size &&
        parser->file->content[parser->parsed] == '\n'
    ) {
        tlc_parse_config_val_on_end(parser);
        tlc_parse_config_on_newline(parser);

        parser->state = TLC_CONFIG_PARSER_STATE_KEY;
        parser->word_start = parser->parsed + 2;
        parser->parsed++;
    } else {
        // single \r is not allowed
        tlc_config_unexpected_symbol(parser, '\r');
    }
}

static inline void tlc_parse_config_eq(p *parser, uc c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
            return; // skip
        case '\n':
            tlc_parse_config_on_newline(parser);
            return;
        case '=':
            parser->state = TLC_CONFIG_PARSER_STATE_VAL;
            parser->word_start = parser->parsed + 1;
            return;
        default:
            tlc_config_unexpected_symbol(parser, c);
    }
}

static inline void tlc_parse_config_val(p *parser, uc c) {
    switch (c) {
        case ' ':
        case '\t':
            tlc_parse_config_val_on_space(parser);
            return;
        case '\r':
            tlc_parse_config_val_on_r(parser);
            return;
        case '\n':
            tlc_parse_config_val_on_end(parser);
            tlc_parse_config_on_newline(parser);
            parser->state = TLC_CONFIG_PARSER_STATE_KEY;
            parser->word_start = parser->parsed + 1;
            return;
        default:
            return; // skip
    }
}

static inline void tlc_parse_config_key(p *parser, uc c) {
    switch (c) {
        case '_':
        TLC_CASE_ALPHA_LOWER:
            return; // skip
        case ' ':
        case '\t':
        case '\r':
            tlc_parse_config_key_on_space(parser);
            return;
        case '\n':
            tlc_parse_config_key_on_space(parser);
            tlc_parse_config_on_newline(parser);
            return;
        case '=':
            tlc_parse_config_key_on_end(parser);
            parser->state = TLC_CONFIG_PARSER_STATE_VAL;
            parser->word_start = parser->parsed + 1;
            return;
        default:
            tlc_config_unexpected_symbol(parser, c);
    }
}

static void tlc_parse_config_file(tlc_file *file, tlc_config *config) {
    tlc_config_parser parser = {
        .config = config,
        .file = file,
        .state = TLC_CONFIG_PARSER_STATE_KEY,
        .line_n = 1,
        .pos_n = 0, // auto incr before iter, so effectively starts at 1
        .parsed = 0, // auto incr before iter, so effectively starts at 1
        .word_start = 1,
    };

    while (parser.parsed < file->size) {
        unsigned char c = file->content[parser.parsed];

        parser.parsed++;
        parser.pos_n++;

        switch (parser.state) {
            case TLC_CONFIG_PARSER_STATE_KEY:
                tlc_parse_config_key(&parser, c);
                break;
            case TLC_CONFIG_PARSER_STATE_EQ:
                tlc_parse_config_eq(&parser, c);
                break;
            case TLC_CONFIG_PARSER_STATE_VAL:
                tlc_parse_config_val(&parser, c);
                break;
            default:
                tlc_config_error(&parser, "unknown state: %d", parser.state);
        }
    }

    if (parser.state != TLC_CONFIG_PARSER_STATE_KEY) {
        tlc_config_error(&parser, "%s", "unfinished key-value pair");
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
        tlc_parse_config_file(file, config);
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
