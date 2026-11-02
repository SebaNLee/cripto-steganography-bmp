#include "lib/bit.h"
#include "steg/steg.h"
#include <string.h>

static size_t lsb4Capacity(size_t bodyLen)
{
    return bodyLen / 0x02;
}

static StegResult lsb4Embed(uint8_t *body, size_t bodyLen, uint8_t *data, size_t dataLen)
{
    if (dataLen > lsb4Capacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }

    size_t nibbleCount = dataLen * 0x02;

    for (size_t i = 0; i < nibbleCount; i++)
    {
        body[i] = lowNibbleSet(body[i], streamNibbleGet(data, i));
    }

    return STEG_OK;
}

static StegResult lsb4Extract(uint8_t *body, size_t bodyLen, uint8_t *out, size_t outLen)
{
    if (outLen > lsb4Capacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }
    if (outLen == 0)
    {
        return STEG_OK;
    }

    memset(out, 0, outLen);

    size_t nibbleCount = outLen * 0x02;

    for (size_t i = 0; i < nibbleCount; i++)
    {
        streamNibbleSet(out, i, lowNibbleGet(body[i]));
    }

    return STEG_OK;
}

StegOps lsb4Ops = {
    .capacity = lsb4Capacity,
    .embed = lsb4Embed,
    .extract = lsb4Extract,
};
