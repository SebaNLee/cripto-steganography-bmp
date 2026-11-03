#include "steg/steg.h"

static StegOps *steg_registry[STEG_ALGO_COUNT] = {
    [LSB1] = &lsb1_ops,
    [LSB4] = &lsb4_ops,
    [LSBI] = &lsbi_ops,
};

StegOps *steg_get_ops(StegAlgo algo)
{
    if ((size_t)algo >= (size_t)STEG_ALGO_COUNT)
    {
        return NULL;
    }

    return steg_registry[algo];
}

size_t steg_capacity(StegAlgo algo, size_t body_len)
{
    StegOps *ops = steg_get_ops(algo);

    if (ops == NULL)
    {
        return 0;
    }

    return ops->capacity(body_len);
}

StegResult steg_embed(StegAlgo algo, uint8_t *body, size_t body_len, uint8_t *data, size_t data_len)
{
    StegOps *ops = steg_get_ops(algo);

    if (ops == NULL)
    {
        return STEG_ERR_UNSUPPORTED;
    }
    if (body == NULL || (data_len > 0 && data == NULL))
    {
        return STEG_ERR_INVALID_ARG;
    }
    if (data_len > ops->capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }

    return ops->embed(body, body_len, data, data_len);
}

StegResult steg_extract(StegAlgo algo, uint8_t *body, size_t body_len, uint8_t *out, size_t out_len)
{
    StegOps *ops = steg_get_ops(algo);

    if (ops == NULL)
    {
        return STEG_ERR_UNSUPPORTED;
    }
    if (body == NULL || (out_len > 0 && out == NULL))
    {
        return STEG_ERR_INVALID_ARG;
    }
    if (out_len > ops->capacity(body_len))
    {
        return STEG_ERR_CAPACITY;
    }

    return ops->extract(body, body_len, out, out_len);
}

char *steg_error_string(StegResult error)
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
