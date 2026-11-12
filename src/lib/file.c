#include "lib/file.h"

#include <stdio.h>
#include <stdlib.h>

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

    file = fopen(path, "rb"); // binary mode read
    if (file == NULL)
    {
        return FILE_ERR_IO;
    }

    fseek(file, 0, SEEK_END);
    end = ftell(file);
    fseek(file, 0, SEEK_SET);

    size = (size_t)end;
    buf = malloc(size);

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

    if (path == NULL || (len > 0 && data == NULL))
    {
        return FILE_ERR_INVALID_ARG;
    }

    file = fopen(path, "wb"); // binary write mode
    if (file == NULL)
    {
        return FILE_ERR_IO;
    }

    if (len > 0 && fwrite(data, 1, len, file) != len)
    {
        fclose(file);
        return FILE_ERR_IO;
    }

    if (fclose(file) != 0)
    {
        return FILE_ERR_IO;
    }

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
