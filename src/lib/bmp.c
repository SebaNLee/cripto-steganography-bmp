#include "lib/bmp.h"

#include <string.h>

#define BMP_HEADER_SIZE 54
#define BMP_INFO_HEADER_SIZE 40
#define BMP_BITS_PER_PIXEL 24
#define BMP_NO_COMPRESSION 0

#define BMP_OFFSET_PIXELS 10
#define BMP_OFFSET_INFO_HEADER_SIZE 14
#define BMP_OFFSET_BITS_PER_PIXEL 28
#define BMP_OFFSET_COMPRESSION 30

static uint16_t read_le16(const uint8_t *buf)
{
    return (uint16_t)(buf[0] | (buf[1] << 8));
}

static uint32_t read_le32(const uint8_t *buf)
{
    return (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
}

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

    // only plain V3 files: 54 byte header, pixels right after it
    if (read_le32(data + BMP_OFFSET_PIXELS) != BMP_HEADER_SIZE ||
        read_le32(data + BMP_OFFSET_INFO_HEADER_SIZE) != BMP_INFO_HEADER_SIZE ||
        read_le16(data + BMP_OFFSET_BITS_PER_PIXEL) != BMP_BITS_PER_PIXEL ||
        read_le32(data + BMP_OFFSET_COMPRESSION) != BMP_NO_COMPRESSION)
    {
        return BMP_ERR_UNSUPPORTED;
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
        return "not a valid BMP file";
    case BMP_ERR_UNSUPPORTED:
        return "unsupported BMP (uncompressed 24-bit v3 required)";
    default:
        return "unknown error";
    }
}
