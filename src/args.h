#ifndef TLC_ARGS_H
#define TLC_ARGS_H

#include "error.h"

#define TLC_ARGS_STATE_TOKEN 1
#define TLC_ARGS_STATE_CONFIG 2
#define TLC_ARGS_STATE_INPUT 3
#define TLC_ARGS_STATE_OUTPUT 4

typedef enum {
    TLC_ARGS_TOKEN_HELP = 1,
    TLC_ARGS_TOKEN_VERSION,
    TLC_ARGS_TOKEN_CONFIG,
    TLC_ARGS_TOKEN_INPUT,
    TLC_ARGS_TOKEN_OUTPUT,
} tlc_args_token;

typedef struct {
    int print_help;
    int print_version;
    const char *config_path; // alias of argv, do not deallocate
    const char *input_path; // alias of argv, do not deallocate
    const char *output_path; // alias of argv, do not deallocate
} tlc_args;

tlc_args *tlc_parse_args(int argc, const char **argv);
void tlc_check_args(tlc_args *args);
void tlc_print_args(tlc_args *args);
void tlc_free_args(tlc_args *args);

#endif
