#ifndef TLC_ARGS_H
#define TLC_ARGS_H

#include "error.h"

#define TLC_ARGS_KEYS_N 6

typedef enum {
    TLC_ARGS_KEY_HELP = 1,
    TLC_ARGS_KEY_VERSION,
    TLC_ARGS_KEY_CONFIG,
    TLC_ARGS_KEY_INPUT,
    TLC_ARGS_KEY_OUTPUT,
    TLC_ARGS_KEY_SERVER,
} tlc_args_key;

typedef enum {
    TLC_ARGS_PARSER_STATE_KEY = 1,
    TLC_ARGS_PARSER_STATE_VALUE,
} tlc_args_parser_state;

typedef struct {
    int server;
    int print_help;
    int print_version;
    const char *config_path; // alias of argv, do not deallocate
    const char *input_path; // alias of argv, do not deallocate
    const char *output_path; // alias of argv, do not deallocate
} tlc_args;

typedef struct {
    tlc_args *args;
    tlc_args_parser_state state;
    tlc_args_key key;
    int seen_keys[1 + TLC_ARGS_KEYS_N];
} tlc_args_parser;

tlc_args *tlc_parse_args(int argc, const char **argv);
void tlc_check_args(tlc_args *args);
void tlc_print_args(tlc_args *args);
void tlc_free_args(tlc_args *args);

#endif
