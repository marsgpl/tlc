#include <string.h>
#include "config.h"

#define MIN_WORD_LENGTH 15
#define MAX_WORD_LENGTH 25
#define MAX_HASH_VALUE 2

int tlc_config_key_id(const char *str, int len) {
    static const unsigned char asso_values[] = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 2, 3, 3, 3, 1,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 0, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3
    };

    static const int lengths[] = {
        15, 25, 15
    };

    static const char *values[] = {
        "allow_semicolon",
        "allow_caret_return_symbol",
        "remove_comments"
    };

    static const int ids[] = {
        TLC_CONFIG_KEY_ALLOW_SEMICOLON,
        TLC_CONFIG_KEY_ALLOW_R,
        TLC_CONFIG_KEY_RM_COMMENTS,
    };

    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        int hash = asso_values[(unsigned char)str[6]];

        if (hash <= MAX_HASH_VALUE && len == lengths[hash]) {
            const char *value = values[hash];

            if (*value == *str && memcmp(value + 1, str + 1, len - 1) == 0) {
                return ids[hash];
            }
        }
    }

    return 0;
}
