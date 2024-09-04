#ifndef TLC_ERROR_H
#define TLC_ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

void tlc_error(const char *fmt, ...);
void tlc_error_errno(const char *fmt, ...);
void *tlc_malloc_or_error(size_t size);
void *tlc_calloc_or_error(size_t size);
void *tlc_realloc_or_error(void *ptr, size_t size);
void tlc_print_escaped_string(const char *str, size_t len, size_t max);

#endif
