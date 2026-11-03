#include "lib/bit.h"
#include "steg/steg.h"
#include <string.h>

#define LSBI_DATA_OFFSET 0x04
#define LSBI_MAP_BITS 0x04
#define LSBI_CHANNELS 0x03
#define LSBI_RED_CHANNEL 0x02

static uint8_t lsbi_pattern(uint8_t value)
{
    uint8_t bit2 = (value >> 0x02) & 0x01;
    uint8_t bit1 = (value >> 0x01) & 0x01;

    return (bit2 << 0x01) | bit1;
}

static size_t lsbi_usable_count(size_t body_len)
{
    size_t count = 0;

    for (size_t i = LSBI_DATA_OFFSET; i < body_len; i++)
    {
        if ((i % LSBI_CHANNELS) != LSBI_RED_CHANNEL)
        {
            count++;
        }
    }

    return count;
}

static size_t lsbi_capacity(size_t body_len)
{
    return lsbi_usable_count(body_len) / 0x08;
}

static StegResult lsbi_embed(uint8_t *body, size_t body_len, uint8_t *data, size_t data_len)
{
    if (data_len > lsbi_capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }
    if (data_len == 0)
    {
        return STEG_OK;
    }

    size_t changed[LSBI_MAP_BITS] = {0};
    size_t same[LSBI_MAP_BITS] = {0};
    size_t bit_count = data_len * 0x08;
    size_t bit_index = 0;

    // first pass, counts per pattern
    for (size_t i = LSBI_DATA_OFFSET; i < body_len && bit_index < bit_count; i++)
    {
        if ((i % LSBI_CHANNELS) == LSBI_RED_CHANNEL)
        {
            continue;
        }

        uint8_t pattern = lsbi_pattern(body[i]);
        uint8_t cover_bit = lsb_get(body[i]);
        uint8_t data_bit = stream_bit_get(data, bit_index);

        if (cover_bit != data_bit)
        {
            changed[pattern]++;
        }
        else
        {
            same[pattern]++;
        }

        bit_index++;
    }

    uint8_t map[LSBI_MAP_BITS];

    for (size_t i = 0; i < LSBI_MAP_BITS; i++)
    {
        map[i] = (changed[i] > same[i]) ? 0x01 : 0x00;
        body[i] = lsb_set(body[i], map[i]);
    }

    bit_index = 0;

    // second pass, write to bmp (XOR with map)
    for (size_t i = LSBI_DATA_OFFSET; i < body_len && bit_index < bit_count; i++)
    {
        if ((i % LSBI_CHANNELS) == LSBI_RED_CHANNEL)
        {
            continue;
        }

        uint8_t pattern = lsbi_pattern(body[i]);
        uint8_t data_bit = stream_bit_get(data, bit_index);

        body[i] = lsb_set(body[i], data_bit ^ map[pattern]);
        bit_index++;
    }

    return STEG_OK;
}

static StegResult lsbi_extract(uint8_t *body, size_t body_len, uint8_t *out, size_t out_len)
{
    if (out_len > lsbi_capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }
    if (out_len == 0)
    {
        return STEG_OK;
    }

    uint8_t map[LSBI_MAP_BITS];

    for (size_t i = 0; i < LSBI_MAP_BITS; i++)
    {
        map[i] = lsb_get(body[i]);
    }

    memset(out, 0, out_len);

    size_t bit_count = out_len * 0x08;
    size_t bit_index = 0;

    for (size_t i = LSBI_DATA_OFFSET; i < body_len && bit_index < bit_count; i++)
    {
        if ((i % LSBI_CHANNELS) == LSBI_RED_CHANNEL)
        {
            continue;
        }

        uint8_t pattern = lsbi_pattern(body[i]);
        uint8_t bit = lsb_get(body[i]) ^ map[pattern];

        stream_bit_set(out, bit_index, bit);
        bit_index++;
    }

    if (bit_index != bit_count)
    {
        return STEG_ERR_CAPACITY;
    }

    return STEG_OK;
}

StegOps lsbi_ops = {
    .capacity = lsbi_capacity,
    .embed = lsbi_embed,
    .extract = lsbi_extract,
};
