#ifndef TLC_CONFIG_H
#define TLC_CONFIG_H

#include <ctype.h>
#include "error.h"
#include "file.h"
#include "case.h"

#define TLC_CONFIG_PATH_DEFAULT "tlc.conf"

#define TLC_CONFIG_DEFAULT_RM_COMMENTS 0
#define TLC_CONFIG_DEFAULT_ALLOW_SEMICOLON 1
#define TLC_CONFIG_DEFAULT_ALLOW_R 1

typedef enum {
    TLC_CONFIG_KEY_RM_COMMENTS = 1,
    TLC_CONFIG_KEY_ALLOW_SEMICOLON,
    TLC_CONFIG_KEY_ALLOW_R,
} tlc_config_key;

typedef enum {
    TLC_CONFIG_PARSER_STATE_KEY = 1,
    TLC_CONFIG_PARSER_STATE_EQ,
    TLC_CONFIG_PARSER_STATE_VAL,
} tlc_config_parser_state;

typedef struct {
    const char *path; // alias, do not deallocate
    int rm_comments;
    int allow_semicolon;
    int allow_r;
} tlc_config;

typedef struct {
    tlc_config *config;
    tlc_file *file;
    tlc_config_parser_state state;
    int line_n;
    int pos_n;
    size_t parsed;
    size_t word_start;
    tlc_config_key key;
} tlc_config_parser;

tlc_config *tlc_init_config(const char *path);
void tlc_check_config(tlc_config *config);
void tlc_print_config(tlc_config *config);
void tlc_free_config(tlc_config *config);

#endif
