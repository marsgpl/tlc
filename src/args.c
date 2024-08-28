#include "args.h"
#include "args.hash.c"

static int tlc_parse_args_config(tlc_args *args, const char *arg) {
    if (args->config_path != NULL) {
        tlc_error("args: multiple config paths are not supported");
    }
    args->config_path = arg;
    return TLC_ARGS_STATE_TOKEN;
}

static int tlc_parse_args_input(tlc_args *args, const char *arg) {
    if (args->input_path != NULL) {
        tlc_error("args: multiple source paths are not supported");
    }
    args->input_path = arg;
    return TLC_ARGS_STATE_TOKEN;
}

static int tlc_parse_args_output(tlc_args *args, const char *arg) {
    if (args->output_path != NULL) {
        tlc_error("args: multiple destination paths are not supported");
    }
    args->output_path = arg;
    return TLC_ARGS_STATE_TOKEN;
}

static int tlc_parse_args_token(tlc_args *args, const char *arg) {
    if (arg[0] != '-') {
        return tlc_parse_args_input(args, arg);
    }

    int token_id = tlc_args_token_id(arg, strlen(arg));

    switch (token_id) {
        case TLC_ARGS_TOKEN_HELP:
            args->print_help = 1;
            return TLC_ARGS_STATE_TOKEN;
        case TLC_ARGS_TOKEN_VERSION:
            args->print_version = 1;
            return TLC_ARGS_STATE_TOKEN;
        case TLC_ARGS_TOKEN_CONFIG:
            return TLC_ARGS_STATE_CONFIG;
        case TLC_ARGS_TOKEN_INPUT:
            return TLC_ARGS_STATE_INPUT;
        case TLC_ARGS_TOKEN_OUTPUT:
            return TLC_ARGS_STATE_OUTPUT;
        default:
            tlc_error("args: unsupported argument: %s; try: tlc --help", arg);
            return 0;
    }
}

tlc_args *tlc_parse_args(int argc, const char **argv) {
    tlc_args *args = tlc_calloc_or_error(sizeof(tlc_args));

    int state = TLC_ARGS_STATE_TOKEN;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        switch (state) {
            case TLC_ARGS_STATE_TOKEN:
                state = tlc_parse_args_token(args, arg);
                break;
            case TLC_ARGS_STATE_CONFIG:
                state = tlc_parse_args_config(args, arg);
                break;
            case TLC_ARGS_STATE_INPUT:
                state = tlc_parse_args_input(args, arg);
                break;
            case TLC_ARGS_STATE_OUTPUT:
                state = tlc_parse_args_output(args, arg);
                break;
            default:
                tlc_error("unknown args parser state: %d", state);
        }
    }

    if (state != TLC_ARGS_STATE_TOKEN) {
        tlc_error("args: unfinished arg sequence");
    }

    return args;
}

void tlc_check_args(tlc_args *args) {
    if (args->print_help || args->print_version) {
        return; // ok
    }

    if (args->config_path != NULL && args->config_path[0] == 0) {
        tlc_error("args: config path is empty");
    }

    if (args->input_path == NULL) {
        tlc_error("args: source path is not provided");
    }

    if (args->input_path[0] == 0) {
        tlc_error("args: source path is empty");
    }

    if (args->output_path != NULL && args->output_path[0] == 0) {
        tlc_error("args: destination path is empty");
    }
}

void tlc_print_args(tlc_args *args) {
    printf("args:\n");
    printf("    print_help: %d\n", args->print_help);
    printf("    print_version: %d\n", args->print_version);
    printf("    config_path: %s\n", args->config_path);
    printf("    input_path: %s\n", args->input_path);
    printf("    output_path: %s\n", args->output_path);
}

void tlc_free_args(tlc_args *args) {
    free(args);
}
