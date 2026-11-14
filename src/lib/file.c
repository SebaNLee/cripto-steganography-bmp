#include "lib/file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TMP_SUFFIX_SIZE 16

FileResult file_read_all(const char *path, uint8_t **out, size_t *out_len)
{
    FILE *file;
    uint8_t *buf;
    long end;
    size_t size;

    if (path == NULL || out == NULL || out_len == NULL)
    {
        return FILE_ERR_INVALID_ARG;
    }

    *out = NULL;
    *out_len = 0;

    file = fopen(path, "rb");
    if (file == NULL)
    {
        return FILE_ERR_IO;
    }

    // seeking fails on directories and pipes, which have no usable size
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return FILE_ERR_IO;
    }

    end = ftell(file);
    if (end < 0 || fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return FILE_ERR_IO;
    }

    size = (size_t)end;
    buf = malloc(size > 0 ? size : 1);

    if (buf == NULL)
    {
        fclose(file);
        return FILE_ERR_NO_MEMORY;
    }

    if (size > 0 && fread(buf, 1, size, file) != size)
    {
        free(buf);
        fclose(file);
        return FILE_ERR_IO;
    }

    if (fclose(file) != 0)
    {
        free(buf);
        return FILE_ERR_IO;
    }

    *out = buf;
    *out_len = size;

    return FILE_OK;
}

FileResult file_write_all(const char *path, const uint8_t *data, size_t len)
{
    FILE *file;
    char *tmp_path;
    size_t tmp_len;

    if (path == NULL || (len > 0 && data == NULL))
    {
        return FILE_ERR_INVALID_ARG;
    }

    tmp_len = strlen(path) + TMP_SUFFIX_SIZE;
    tmp_path = malloc(tmp_len);
    if (tmp_path == NULL)
    {
        return FILE_ERR_NO_MEMORY;
    }

    snprintf(tmp_path, tmp_len, "%s.tmp.%u", path, (unsigned)getpid());

    file = fopen(tmp_path, "wb");
    if (file == NULL)
    {
        free(tmp_path);
        return FILE_ERR_IO;
    }

    if (len > 0 && fwrite(data, 1, len, file) != len)
    {
        fclose(file);
        remove(tmp_path);
        free(tmp_path);
        return FILE_ERR_IO;
    }

    if (fclose(file) != 0)
    {
        remove(tmp_path);
        free(tmp_path);
        return FILE_ERR_IO;
    }

    if (rename(tmp_path, path) != 0)
    {
        remove(tmp_path);
        free(tmp_path);
        return FILE_ERR_IO;
    }

    free(tmp_path);
    return FILE_OK;
}

const char *file_error_string(FileResult result)
{
    switch (result)
    {
    case FILE_OK:
        return "ok";
    case FILE_ERR_INVALID_ARG:
        return "invalid file argument";
    case FILE_ERR_IO:
        return "could not read or write file";
    case FILE_ERR_NO_MEMORY:
        return "out of memory";
    default:
        return "unknown error";
    }
}
