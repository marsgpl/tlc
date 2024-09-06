#include "args.h"
#include "args.hash.c"

typedef tlc_args_parser p;

#define error(msg, ...) \
    tlc_error("args: " msg __VA_OPT__(,) __VA_ARGS__)

static inline void on_input_path(p *parser, const char *path) {
    if (parser->args->input_path != NULL) {
        error("multiple source paths are not supported");
    }

    parser->args->input_path = path;
}

static inline void on_value(p *parser, const char *arg) {
    parser->state = TLC_ARGS_PARSER_STATE_KEY;

    switch (parser->key) {
        case TLC_ARGS_KEY_CONFIG:
            parser->args->config_path = arg;
            return;
        case TLC_ARGS_KEY_INPUT:
            on_input_path(parser, arg);
            return;
        case TLC_ARGS_KEY_OUTPUT:
            parser->args->output_path = arg;
            return;
        default:
            error("unknown key id: %d", parser->key);
    }
}

static inline void on_key(p *parser, const char *arg) {
    if (arg[0] != '-') {
        on_input_path(parser, arg);
        return;
    }

    int key = tlc_arg_key_id(arg, strlen(arg));

    if (!key) {
        error("unsupported argument: %s; try: tlc --help", arg);
    }

    if (parser->seen_keys[key]) {
        error("duplicate argument: %s", arg);
    }

    parser->seen_keys[key] = 1;

    switch (key) {
        case TLC_ARGS_KEY_HELP:
            parser->args->print_help = 1;
            return;
        case TLC_ARGS_KEY_VERSION:
            parser->args->print_version = 1;
            return;
        case TLC_ARGS_KEY_SERVER:
            parser->args->server = 1;
            return;
        default:
            parser->state = TLC_ARGS_PARSER_STATE_VALUE;
            parser->key = key;
            return;
    }
}

tlc_args *tlc_parse_args(int argc, const char **argv) {
    tlc_args *args = tlc_calloc_or_error(sizeof(tlc_args));

    tlc_args_parser parser = {
        .args = args,
        .state = TLC_ARGS_PARSER_STATE_KEY,
    };

    memset(parser.seen_keys, 0, sizeof(parser.seen_keys));

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        switch (parser.state) {
            case TLC_ARGS_PARSER_STATE_KEY:
                on_key(&parser, arg);
                break;
            case TLC_ARGS_PARSER_STATE_VALUE:
                on_value(&parser, arg);
                break;
            default:
                error("unknown parser state: %d", parser.state);
        }
    }

    if (parser.state != TLC_ARGS_PARSER_STATE_KEY) {
        error("unfinished key-value pair");
    }

    return args;
}

void tlc_check_args(tlc_args *args) {
    if (args->print_help || args->print_version || args->server) {
        return;
    }

    if (args->config_path != NULL && args->config_path[0] == 0) {
        error("config path is empty");
    }

    if (args->input_path == NULL) {
        error("source path is not provided");
    }

    if (args->input_path[0] == 0) {
        error("source path is empty");
    }

    if (args->output_path != NULL && args->output_path[0] == 0) {
        error("destination path is empty");
    }
}

void tlc_print_args(tlc_args *args) {
    printf("args:\n");
    printf("    server: %d\n", args->server);
    printf("    print_help: %d\n", args->print_help);
    printf("    print_version: %d\n", args->print_version);
    printf("    config_path: %s\n", args->config_path);
    printf("    input_path: %s\n", args->input_path);
    printf("    output_path: %s\n", args->output_path);
}

void tlc_free_args(tlc_args *args) {
    free(args);
}
