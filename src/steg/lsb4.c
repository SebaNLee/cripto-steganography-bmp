#include "lib/bit.h"
#include "steg/steg.h"
#include <string.h>

static size_t lsb4_capacity(size_t body_len)
{
    return body_len / 0x02;
}

static StegResult lsb4_embed(uint8_t *body, size_t body_len, uint8_t *data, size_t data_len)
{
    if (data_len > lsb4_capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }

    size_t nibble_count = data_len * 0x02;

    for (size_t i = 0; i < nibble_count; i++)
    {
        body[i] = low_nibble_set(body[i], stream_nibble_get(data, i));
    }

    return STEG_OK;
}

static StegResult lsb4_extract(uint8_t *body, size_t body_len, uint8_t *out, size_t out_len)
{
    if (out_len > lsb4_capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }
    if (out_len == 0)
    {
        return STEG_OK;
    }

    memset(out, 0, out_len);

    size_t nibble_count = out_len * 0x02;

    for (size_t i = 0; i < nibble_count; i++)
    {
        stream_nibble_set(out, i, low_nibble_get(body[i]));
    }

    return STEG_OK;
}

StegOps lsb4_ops = {
    .capacity = lsb4_capacity,
    .embed = lsb4_embed,
    .extract = lsb4_extract,
};
