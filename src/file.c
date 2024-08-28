#include "file.h"

int tlc_file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

tlc_file *tlc_read_file(const char *path) {
    int fd = open(path, O_RDONLY | O_NOATIME | O_NOFOLLOW);

    if (fd == -1) {
        tlc_error_errno("open failed; path: %s", path);
    }

    struct stat st;

    if (fstat(fd, &st) == -1) {
        tlc_error_errno("fstat failed; path: %s", path);
    }

    if (!S_ISREG(st.st_mode)) {
        tlc_error("not a regular file; type: %s; path: %s",
            tlc_file_type(st.st_mode), path);
    }

    if (st.st_size == 0) {
        tlc_error("file is empty; path: %s", path);
    }

    void *content = mmap(NULL, st.st_size, PROT_READ,
        MAP_PRIVATE | MAP_NORESERVE | MAP_NONBLOCK | MAP_UNINITIALIZED,
        fd, 0);

    if (content == MAP_FAILED) {
        tlc_error_errno("mmap failed; path: %s; size: %lld", path, st.st_size);
    }

    if (close(fd) != 0) {
        tlc_error_errno("close failed; fd: %d; path: %s", fd, path);
    }

    tlc_file *file = tlc_malloc_or_error(sizeof(tlc_file));

    file->path = path;
    file->size = st.st_size;
    file->content = content;

    return file;
}

void tlc_print_file(tlc_file *file, const char *label) {
    printf("%s:\n", label == NULL ? "file" : label);
    printf("    path: %s\n", file->path);

    size_t max_bytes = 32;

    if (max_bytes < file->size) {
        printf("    content (first %lu byte%s): ",
            max_bytes,
            max_bytes == 1 ? "" : "s");
    } else {
        printf("    content: ");
    }

    tlc_print_escaped_string(file->content, file->size, 32);
    printf("\n");

    printf("    size: %lu\n", file->size);
}

void tlc_free_file(tlc_file *file) {
    if (munmap((void *)file->content, file->size) != 0) {
        tlc_error_errno("munmap failed; path: %s; size: %lu",
            file->path,
            file->size);
    }

    free(file);
}
