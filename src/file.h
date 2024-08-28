#ifndef TLC_FILE_H
#define TLC_FILE_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "error.h"

#ifndef O_NOATIME
#define O_NOATIME 0
#endif

#ifndef MAP_NONBLOCK
#define MAP_NONBLOCK 0
#endif

#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED 0
#endif

#define tlc_file_type(m) ( \
    S_ISREG(m) ? "regular file" : \
    S_ISDIR(m) ? "directory" : \
    S_ISCHR(m) ? "character device" : \
    S_ISBLK(m) ? "block device" : \
    S_ISFIFO(m) ? "named pipe (fifo)" : \
    S_ISLNK(m) ? "symbolic link" : \
    S_ISSOCK(m) ? "socket" : \
    "unknown")

typedef struct {
    const char *path; // alias, do not deallocate
    const char *content;
    size_t size;
} tlc_file;

int tlc_file_exists(const char *path);
tlc_file *tlc_read_file(const char *path);
void tlc_print_file(tlc_file *file, const char *label);
void tlc_free_file(tlc_file *file);

#endif
