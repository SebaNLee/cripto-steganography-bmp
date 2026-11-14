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

    uint32_t declared_size;      // bfSize crudo
    uint32_t image_size;         // biSizeImage crudo, 0 si no aplica
    uint32_t pixels_offset;      // offset real de los píxeles

    int declared_size_valid;     // bfSize >= pixels_offset && bfSize <= size
    size_t trailing_offset;      // bfSize si es válido; size si no
    size_t trailing_len;         // size - bfSize si es válido; 0 si no
} Bmp;

BmpResult bmp_parse(uint8_t *data, size_t size, Bmp *bmp);

const char *bmp_error_string(BmpResult result);

#endif
