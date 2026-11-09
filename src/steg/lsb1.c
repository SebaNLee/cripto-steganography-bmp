#include "lib/bit.h"
#include "steg/steg.h"
#include <string.h>

static size_t lsb1_capacity(size_t body_len)
{
    return body_len / 0x08;
}

static StegResult lsb1_embed(uint8_t *body, size_t body_len, uint8_t *data, size_t data_len)
{
    if (data_len > lsb1_capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }

    size_t bit_count = data_len * 0x08;

    for (size_t i = 0; i < bit_count; i++)
    {
        // note: do not confuse bit and byte operations
        // set i-th byte from body the value of the i-th bit of data
        body[i] = lsb_set(body[i], stream_bit_get(data, i));
    }

    return STEG_OK;
}

static StegResult lsb1_extract(uint8_t *body, size_t body_len, uint8_t *out, size_t out_len)
{
    if (out_len > lsb1_capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }
    if (out_len == 0)
    {
        return STEG_OK;
    }

    memset(out, 0, out_len);

    size_t bit_count = out_len * 0x08;

    for (size_t i = 0; i < bit_count; i++)
    {
        stream_bit_set(out, i, lsb_get(body[i]));
    }

    return STEG_OK;
}

StegOps lsb1_ops = {
    .capacity = lsb1_capacity,
    .embed = lsb1_embed,
    .extract = lsb1_extract,
};
