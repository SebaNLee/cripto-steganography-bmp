#include "crypto.h"

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
