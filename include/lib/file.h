#ifndef FILE_H
#define FILE_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    FILE_OK = 0,
    FILE_ERR_INVALID_ARG,
    FILE_ERR_IO,
    FILE_ERR_NO_MEMORY
} FileResult;

FileResult file_read_all(const char *path, uint8_t **out, size_t *out_len);

FileResult file_write_all(const char *path, const uint8_t *data, size_t len);

const char *file_error_string(FileResult result);

#endif
