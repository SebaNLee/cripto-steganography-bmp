#include "lib/bit.h"
#include "steg/steg.h"
#include <string.h>

static size_t lsb1Capacity(size_t bodyLen)
{
    return bodyLen / 0x08;
}

static StegResult lsb1Embed(uint8_t *body, size_t bodyLen, uint8_t *data, size_t dataLen)
{
    if (dataLen > lsb1Capacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }

    size_t bitCount = dataLen * 0x08;

    for (size_t i = 0; i < bitCount; i++)
    {
        // note: do not confuse bit and byte operations
        // set i-th byte from body the value of the i-th bit of data
        body[i] = lsbSet(body[i], streamBitGet(data, i));
    }

    return STEG_OK;
}

static StegResult lsb1Extract(uint8_t *body, size_t bodyLen, uint8_t *out, size_t outLen)
{
    if (outLen > lsb1Capacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }
    if (outLen == 0)
    {
        return STEG_OK;
    }

    memset(out, 0, outLen);

    size_t bitCount = outLen * 0x08;

    for (size_t i = 0; i < bitCount; i++)
    {
        streamBitSet(out, i, lsbGet(body[i]));
    }

    return STEG_OK;
}

StegOps lsb1Ops = {
    .capacity = lsb1Capacity,
    .embed = lsb1Embed,
    .extract = lsb1Extract,
};
