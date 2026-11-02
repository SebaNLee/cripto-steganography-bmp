#include "steg/steg.h"

static StegOps *stegRegistry[STEG_ALGO_COUNT] = {
    [LSB1] = &lsb1Ops,
    [LSB4] = &lsb4Ops,
    [LSBI] = &lsbiOps,
};

StegOps *stegGetOps(StegAlgo algo)
{
    if ((size_t)algo >= (size_t)STEG_ALGO_COUNT)
    {
        return NULL;
    }

    return stegRegistry[algo];
}

size_t stegCapacity(StegAlgo algo, size_t bodyLen)
{
    StegOps *ops = stegGetOps(algo);

    if (ops == NULL)
    {
        return 0;
    }

    return ops->capacity(bodyLen);
}

StegResult stegEmbed(StegAlgo algo, uint8_t *body, size_t bodyLen, uint8_t *data, size_t dataLen)
{
    StegOps *ops = stegGetOps(algo);

    if (ops == NULL)
    {
        return STEG_ERR_UNSUPPORTED;
    }
    if (body == NULL || (dataLen > 0 && data == NULL))
    {
        return STEG_ERR_INVALID_ARG;
    }
    if (dataLen > ops->capacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }

    return ops->embed(body, bodyLen, data, dataLen);
}

StegResult stegExtract(StegAlgo algo, uint8_t *body, size_t bodyLen, uint8_t *out, size_t outLen)
{
    StegOps *ops = stegGetOps(algo);

    if (ops == NULL)
    {
        return STEG_ERR_UNSUPPORTED;
    }
    if (body == NULL || (outLen > 0 && out == NULL))
    {
        return STEG_ERR_INVALID_ARG;
    }
    if (outLen > ops->capacity(bodyLen))
    {
        return STEG_ERR_CAPACITY;
    }

    return ops->extract(body, bodyLen, out, outLen);
}

char *stegErrorString(StegResult error)
{
    switch (error)
    {
    case STEG_OK:
        return "ok";
    case STEG_ERR_INVALID_ARG:
        return "invalid argument";
    case STEG_ERR_UNSUPPORTED:
        return "unsupported algorithm";
    case STEG_ERR_CAPACITY:
        return "insufficient carrier capacity";
    default:
        return "unknown error";
    }
}
