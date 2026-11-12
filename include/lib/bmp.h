#ifndef BMP_H
#define BMP_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    BMP_OK = 0,
    BMP_ERR_INVALID_ARG,
    BMP_ERR_INVALID,
    BMP_ERR_UNSUPPORTED
} BmpResult;

typedef struct
{
    uint8_t *data;
    size_t size;
    uint8_t *body;
    size_t body_len;
} Bmp;

BmpResult bmp_parse(uint8_t *data, size_t size, Bmp *bmp);

const char *bmp_error_string(BmpResult result);

#endif
