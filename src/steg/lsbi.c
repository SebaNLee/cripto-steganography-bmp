#include "lib/bit.h"
#include "steg/steg.h"
#include <string.h>

#define LSBI_DATA_OFFSET 0x04
#define LSBI_MAP_BITS 0x04
#define LSBI_CHANNELS 0x03
#define LSBI_RED_CHANNEL 0x02

static uint8_t lsbiPattern(uint8_t value)
{
    uint8_t bit2 = (value >> 0x02) & 0x01;
    uint8_t bit1 = (value >> 0x01) & 0x01;

    return (bit2 << 0x01) | bit1;
}

static size_t lsbiUsableCount(size_t bodyLen)
{
    size_t count = 0;

    for (size_t i = LSBI_DATA_OFFSET; i < bodyLen; i++)
    {
        if ((i % LSBI_CHANNELS) != LSBI_RED_CHANNEL)
        {
            count++;
        }
    }

    return count;
}

static size_t lsbiCapacity(size_t bodyLen)
{
    return lsbiUsableCount(bodyLen) / 0x08;
}

static StegResult lsbiEmbed(uint8_t *body, size_t bodyLen, uint8_t *data, size_t dataLen)
{
    if (dataLen > lsbiCapacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }
    if (dataLen == 0)
    {
        return STEG_OK;
    }

    size_t changed[LSBI_MAP_BITS] = {0};
    size_t same[LSBI_MAP_BITS] = {0};
    size_t bitCount = dataLen * 0x08;
    size_t bitIndex = 0;

    // first pass, counts per pattern
    for (size_t i = LSBI_DATA_OFFSET; i < bodyLen && bitIndex < bitCount; i++)
    {
        if ((i % LSBI_CHANNELS) == LSBI_RED_CHANNEL)
        {
            continue;
        }

        uint8_t pattern = lsbiPattern(body[i]);
        uint8_t coverBit = lsbGet(body[i]);
        uint8_t dataBit = streamBitGet(data, bitIndex);

        if (coverBit != dataBit)
        {
            changed[pattern]++;
        }
        else
        {
            same[pattern]++;
        }

        bitIndex++;
    }

    uint8_t map[LSBI_MAP_BITS];

    for (size_t i = 0; i < LSBI_MAP_BITS; i++)
    {
        map[i] = (changed[i] > same[i]) ? 0x01 : 0x00;
        body[i] = lsbSet(body[i], map[i]);
    }

    bitIndex = 0;

    // second pass, write to bmp (XOR with map)
    for (size_t i = LSBI_DATA_OFFSET; i < bodyLen && bitIndex < bitCount; i++)
    {
        if ((i % LSBI_CHANNELS) == LSBI_RED_CHANNEL)
        {
            continue;
        }

        uint8_t pattern = lsbiPattern(body[i]);
        uint8_t dataBit = streamBitGet(data, bitIndex);

        body[i] = lsbSet(body[i], dataBit ^ map[pattern]);
        bitIndex++;
    }

    return STEG_OK;
}

static StegResult lsbiExtract(uint8_t *body, size_t bodyLen, uint8_t *out, size_t outLen)
{
    if (outLen > lsbiCapacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }
    if (outLen == 0)
    {
        return STEG_OK;
    }

    uint8_t map[LSBI_MAP_BITS];

    for (size_t i = 0; i < LSBI_MAP_BITS; i++)
    {
        map[i] = lsbGet(body[i]);
    }

    memset(out, 0, outLen);

    size_t bitCount = outLen * 0x08;
    size_t bitIndex = 0;

    for (size_t i = LSBI_DATA_OFFSET; i < bodyLen && bitIndex < bitCount; i++)
    {
        if ((i % LSBI_CHANNELS) == LSBI_RED_CHANNEL)
        {
            continue;
        }

        uint8_t pattern = lsbiPattern(body[i]);
        uint8_t bit = lsbGet(body[i]) ^ map[pattern];

        streamBitSet(out, bitIndex, bit);
        bitIndex++;
    }

    if (bitIndex != bitCount)
    {
        return STEG_ERR_CAPACITY;
    }

    return STEG_OK;
}

StegOps lsbiOps = {
    .capacity = lsbiCapacity,
    .embed = lsbiEmbed,
    .extract = lsbiExtract,
};
