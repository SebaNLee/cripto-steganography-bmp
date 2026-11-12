#include "lib/bmp.h"

#include <string.h>

#define BMP_HEADER_SIZE 54

BmpResult bmp_parse(uint8_t *data, size_t size, Bmp *bmp)
{
    if (bmp == NULL || data == NULL)
    {
        return BMP_ERR_INVALID_ARG;
    }

    memset(bmp, 0, sizeof(*bmp));

    // https://en.wikipedia.org/wiki/BMP_file_format#File_headers
    if (size <= BMP_HEADER_SIZE || data[0] != 'B' || data[1] != 'M')
    {
        return BMP_ERR_INVALID;
    }

    bmp->data = data;
    bmp->size = size;
    bmp->body = data + BMP_HEADER_SIZE;
    bmp->body_len = size - BMP_HEADER_SIZE;

    return BMP_OK;
}

const char *bmp_error_string(BmpResult result)
{
    switch (result)
    {
    case BMP_OK:
        return "ok";
    case BMP_ERR_INVALID_ARG:
        return "invalid BMP argument";
    case BMP_ERR_INVALID:
        return "invalid BMP (uncompressed 24-bit v3 required)";
    default:
        return "unknown error";
    }
}
