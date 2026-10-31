#include "crypto.h"

#include <stdio.h>
#include <string.h>

crypto_algo_t crypto_algo_from_str(const char *s)
{
    if (strcmp(s, "aes128") == 0)
        return CRYPTO_AES128;
    if (strcmp(s, "aes192") == 0)
        return CRYPTO_AES192;
    if (strcmp(s, "aes256") == 0)
        return CRYPTO_AES256;
    if (strcmp(s, "3des") == 0)
        return CRYPTO_3DES;
    return CRYPTO_ALGO_INVALID;
}

crypto_mode_t crypto_mode_from_str(const char *s)
{
    if (strcmp(s, "ecb") == 0)
        return CRYPTO_ECB;
    if (strcmp(s, "cfb") == 0)
        return CRYPTO_CFB;
    if (strcmp(s, "ofb") == 0)
        return CRYPTO_OFB;
    if (strcmp(s, "cbc") == 0)
        return CRYPTO_CBC;
    return CRYPTO_MODE_INVALID;
}

int crypto_enabled(const crypto_params_t *p)
{
    return p->password != NULL;
}

void crypto_apply_defaults(crypto_params_t *p)
{
    if (!crypto_enabled(p))
    {
        if (p->algo != CRYPTO_ALGO_UNSET || p->mode != CRYPTO_MODE_UNSET)
            fprintf(stderr, "stegobmp: warning: -a and -m are ignored without -pass\n");
        return;
    }

    if (p->algo == CRYPTO_ALGO_UNSET)
        p->algo = CRYPTO_AES128;
    if (p->mode == CRYPTO_MODE_UNSET)
        p->mode = CRYPTO_CBC;
}
