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

typedef enum
{
    CRYPTO_OK = 0,
    CRYPTO_ERR_INVALID_CIPHER,
    CRYPTO_ERR_TOO_LARGE,
    CRYPTO_ERR_KEY_DERIVATION,
    CRYPTO_ERR_NO_MEMORY,
    CRYPTO_ERR_ENCRYPT,
    CRYPTO_ERR_DECRYPT,
} CryptoResult;

typedef struct
{
    CryptoAlgo algo;
    CryptoMode mode;
    const char *password;
} CryptoParams;

CryptoAlgo crypto_algo_from_str(const char *s);
CryptoMode crypto_mode_from_str(const char *s);

int crypto_enabled(const CryptoParams *p);
/* -a or -m given without -pass, so they are ignored. */
int crypto_options_ignored(const CryptoParams *p);
void crypto_apply_defaults(CryptoParams *p);

CryptoResult crypto_encrypt(const CryptoParams *p, const uint8_t *in, size_t in_len,
                            uint8_t **out, size_t *out_len);

CryptoResult crypto_decrypt(const CryptoParams *p, const uint8_t *in, size_t in_len,
                            uint8_t **out, size_t *out_len);

const char *crypto_error_string(CryptoResult result);

#endif
