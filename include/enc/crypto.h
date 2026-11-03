#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    CRYPTO_ALGO_UNSET = 0,
    CRYPTO_ALGO_INVALID,
    CRYPTO_AES128,
    CRYPTO_AES192,
    CRYPTO_AES256,
    CRYPTO_3DES,
} CryptoAlgo;

typedef enum
{
    CRYPTO_MODE_UNSET = 0,
    CRYPTO_MODE_INVALID,
    CRYPTO_ECB,
    CRYPTO_CFB,
    CRYPTO_OFB,
    CRYPTO_CBC,
} CryptoMode;

typedef struct
{
    CryptoAlgo algo;
    CryptoMode mode;
    const char *password;
} CryptoParams;

CryptoAlgo crypto_algo_from_str(const char *s);
CryptoMode crypto_mode_from_str(const char *s);

int crypto_enabled(const CryptoParams *p);
void crypto_apply_defaults(CryptoParams *p);

int crypto_encrypt(const CryptoParams *p, const uint8_t *in, size_t in_len, uint8_t **out,
                   size_t *out_len);

int crypto_decrypt(const CryptoParams *p, const uint8_t *in, size_t in_len, uint8_t **out,
                   size_t *out_len);

#endif
