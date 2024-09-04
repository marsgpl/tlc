#include <string.h>
#include "tokens.h"

#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MAX_HASH_VALUE 60

int tlc_keyword_id(const char *str, int len) {
    static const unsigned char asso_values[] = {
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61,  0, 30,
        1,  5,  10, 18, 30, 61,  3, 30, 20, 18,
        0,  25, 23, 61, 10, 10, 15,  0, 30, 61,
        30, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
        61, 61, 61, 61, 61, 61, 61
    };

    static const int lengths[] = {
        4,  3,  0,  2,  0,  4,  0,  0,  0,  0,  6,  3,  0,  2,
        0,  4,  0,  0,  8,  0,  6,  0,  0,  5,  0,  6,  0,  0,
        3,  0,  2,  0,  0,  2,  0,  5,  0,  0,  5,  0,  3,  0,
        0,  0,  0,  3,  0,  0,  0,  0,  5,  0,  0,  0,  0,  4,
        0,  0,  0,  0,  5
    };

    static const char *values[] = {
        "then",
        "and",
        "",
        "in",
        "",
        "true",
        "", "", "", "",
        "return",
        "end",
        "",
        "if",
        "",
        "else",
        "", "",
        "function",
        "",
        "elseif",
        "", "",
        "false",
        "",
        "repeat",
        "", "",
        "for",
        "",
        "do",
        "", "",
        "or",
        "",
        "while",
        "", "",
        "local",
        "",
        "not",
        "", "", "", "",
        "nil",
        "", "", "", "",
        "until",
        "", "", "", "",
        "goto",
        "", "", "", "",
        "break"
    };

    static const int ids[] = {
        TLC_KW_THEN,
        TLC_KW_AND,
        0,
        TLC_KW_IN,
        0,
        TLC_KW_TRUE,
        0, 0, 0, 0,
        TLC_KW_RETURN,
        TLC_KW_END,
        0,
        TLC_KW_IF,
        0,
        TLC_KW_ELSE,
        0, 0,
        TLC_KW_FUNCTION,
        0,
        TLC_KW_ELSEIF,
        0, 0,
        TLC_KW_FALSE,
        0,
        TLC_KW_REPEAT,
        0, 0,
        TLC_KW_FOR,
        0,
        TLC_KW_DO,
        0, 0,
        TLC_KW_OR,
        0,
        TLC_KW_WHILE,
        0, 0,
        TLC_KW_LOCAL,
        0,
        TLC_KW_NOT,
        0, 0, 0, 0,
        TLC_KW_NIL,
        0, 0, 0, 0,
        TLC_KW_UNTIL,
        0, 0, 0, 0,
        TLC_KW_GOTO,
        0, 0, 0, 0,
        TLC_KW_BREAK,
    };

    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        int hash = asso_values[(unsigned char)str[len - 1]]
            + asso_values[(unsigned char)str[0] + 1];

        if (hash <= MAX_HASH_VALUE && len == lengths[hash]) {
            const char *value = values[hash];

            if (*value == *str && memcmp(value + 1, str + 1, len - 1) == 0) {
                return ids[hash];
            }
        }
    }

    return 0;
}
