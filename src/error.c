#include "error.h"

static const char *escape[256] = {
    "\\x00", "\\x01", "\\x02", "\\x03", "\\x04", "\\x05", "\\x06", "\\a",
    "\\b",   "\\t",   "\\n",   "\\v",   "\\f",   "\\r",   "\\x0e", "\\x0f",
    "\\x10", "\\x11", "\\x12", "\\x13", "\\x14", "\\x15", "\\x16", "\\x17",
    "\\x18", "\\x19", "\\x1a", "\\x1b", "\\x1c", "\\x1d", "\\x1e", "\\x1f",

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, NULL, NULL, NULL, NULL,   NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,   NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,   NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, "\\\\", NULL, NULL, NULL,

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, "\\x7f",

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

void tlc_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "TLC Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(EXIT_FAILURE);
}

void tlc_error_errno(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "TLC Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "; errno: %s (#%d)\n", strerror(errno), errno);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void *tlc_malloc_or_error(size_t size) {
    void *ptr = malloc(size);

    if (ptr == NULL) {
        tlc_error_errno("malloc failed; size: %lu", size);
    }

    return ptr;
}

void *tlc_calloc_or_error(size_t size) {
    void *ptr = calloc(1, size);

    if (ptr == NULL) {
        tlc_error_errno("calloc failed; size: %lu", size);
    }

    return ptr;
}

void *tlc_realloc_or_error(void *ptr, size_t size) {
    ptr = realloc(ptr, size);

    if (ptr == NULL) {
        tlc_error_errno("realloc failed; size: %lu", size);
    }

    return ptr;
}

void tlc_print_escaped_string(const char *str, size_t len, size_t max) {
    if (max > len) {
        max = len;
    }

    for (size_t i = 0; i < max; ++i) {
        unsigned char c = str[i];
        const char *esc = escape[c];

        if (esc != NULL) {
            printf("%s", esc);
        } else {
            printf("%c", c);
        }
    }
}
