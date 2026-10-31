#ifndef CRYPTO_H
#define CRYPTO_H

typedef enum
{
    CRYPTO_ALGO_UNSET = 0,
    CRYPTO_ALGO_INVALID,
    CRYPTO_AES128,
    CRYPTO_AES192,
    CRYPTO_AES256,
    CRYPTO_3DES,
} crypto_algo_t;

typedef enum
{
    CRYPTO_MODE_UNSET = 0,
    CRYPTO_MODE_INVALID,
    CRYPTO_ECB,
    CRYPTO_CFB,
    CRYPTO_OFB,
    CRYPTO_CBC,
} crypto_mode_t;

typedef struct
{
    crypto_algo_t algo;
    crypto_mode_t mode;
    const char *password;
} crypto_params_t;

crypto_algo_t crypto_algo_from_str(const char *s);
crypto_mode_t crypto_mode_from_str(const char *s);

int crypto_enabled(const crypto_params_t *p);
void crypto_apply_defaults(crypto_params_t *p);

#endif
