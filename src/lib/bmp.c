#include "lib/bmp.h"

#include <string.h>

#define BMP_FILE_HEADER_SIZE 14
#define BMP_MIN_HEADER_SIZE 54
#define BMP_MIN_INFO_HEADER_SIZE 40
#define BMP_BITS_PER_PIXEL 24
#define BMP_NO_COMPRESSION 0

#define BMP_OFFSET_PIXELS 10
#define BMP_OFFSET_INFO_HEADER_SIZE 14
#define BMP_OFFSET_BITS_PER_PIXEL 28
#define BMP_OFFSET_COMPRESSION 30
#define BMP_OFFSET_FILE_SIZE 2
#define BMP_OFFSET_IMAGE_SIZE 34

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

    uint32_t pixels_offset;
    uint32_t info_header_size;
    uint32_t declared_size;

    // https://en.wikipedia.org/wiki/BMP_file_format#File_headers
    if (size <= BMP_MIN_HEADER_SIZE || data[0] != 'B' || data[1] != 'M')
    {
        return BMP_ERR_INVALID;
    }

    // frozen.bmp has a V5 header (124 bytes), so the pixels start where the file says, not at 54
    info_header_size = read_le32(data + BMP_OFFSET_INFO_HEADER_SIZE);
    pixels_offset = read_le32(data + BMP_OFFSET_PIXELS);

    if (info_header_size < BMP_MIN_INFO_HEADER_SIZE ||
        read_le16(data + BMP_OFFSET_BITS_PER_PIXEL) != BMP_BITS_PER_PIXEL ||
        read_le32(data + BMP_OFFSET_COMPRESSION) != BMP_NO_COMPRESSION)
    {
        return BMP_ERR_UNSUPPORTED;
    }

    if (info_header_size > size - BMP_FILE_HEADER_SIZE ||
        pixels_offset < BMP_FILE_HEADER_SIZE + info_header_size || pixels_offset >= size)
    {
        return BMP_ERR_INVALID;
    }

    declared_size = read_le32(data + BMP_OFFSET_FILE_SIZE);
    bmp->image_size = read_le32(data + BMP_OFFSET_IMAGE_SIZE);

    // declared_size is only trusted when pixels_offset <= declared_size <= size
    if (declared_size >= pixels_offset && declared_size <= size)
    {
        bmp->declared_size_valid = 1;
        bmp->body_len = (size_t)(declared_size - pixels_offset);
        bmp->trailing_offset = (size_t)declared_size;
        bmp->trailing_len = size - (size_t)declared_size;
    }
    else
    {
        bmp->declared_size_valid = 0;
        bmp->body_len = size - pixels_offset;
        bmp->trailing_offset = size;
        bmp->trailing_len = 0;
    }

    bmp->data = data;
    bmp->size = size;
    bmp->body = data + pixels_offset;

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
        return "unsupported BMP (uncompressed 24-bit required)";
    default:
        return "unknown error";
    }
}
