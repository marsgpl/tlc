#include <string.h>
#include "args.h"

#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 9
#define MAX_HASH_VALUE 19

static int get_hash(const char *str, unsigned int len) {
    static unsigned char asso_values[] = {
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20,  3,
        20, 20, 20,  5,  9, 10, 20, 20, 20, 20,
        15,  0,  5, 20, 20, 20,  5, 20,  4, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20
    };

    register unsigned int hval = 0;

    switch (len) {
        default:
            hval += asso_values[(unsigned char)str[2]];
            /*FALLTHROUGH*/
        case 2:
            break;
    }

    return hval + asso_values[(unsigned char)str[len - 1]];
}

int tlc_arg_key_id(const char *str, int len) {
    static const int lengths[] = {
        2,  0,  0,  2,  2,  8,  0,  0,  8,  2,  2,  0,  0,  0,
        6,  7,  0,  0,  0,  9
    };

    static const char *values[] = {
        "-o",
        "", "",
        "-c",
        "-v",
        "--output",
        "", "",
        "--config",
        "-h",
        "-i",
        "", "", "",
        "--help",
        "--input",
        "", "", "",
        "--version"
    };

    static const int ids[] = {
        TLC_ARGS_KEY_OUTPUT,
        0, 0,
        TLC_ARGS_KEY_CONFIG,
        TLC_ARGS_KEY_VERSION,
        TLC_ARGS_KEY_OUTPUT,
        0, 0,
        TLC_ARGS_KEY_CONFIG,
        TLC_ARGS_KEY_HELP,
        TLC_ARGS_KEY_INPUT,
        0, 0, 0,
        TLC_ARGS_KEY_HELP,
        TLC_ARGS_KEY_INPUT,
        0, 0, 0,
        TLC_ARGS_KEY_VERSION,
    };

    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        int hash = get_hash(str, len);

        if (hash <= MAX_HASH_VALUE && len == lengths[hash]) {
            const char *value = values[hash];

            if (*value == *str && memcmp(value + 1, str + 1, len - 1) == 0) {
                return ids[hash];
            }
        }
    }

    return 0;
}
