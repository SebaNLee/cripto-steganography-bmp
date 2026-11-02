#include "crypto.h"

#include <openssl/evp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PBKDF2_ITERATIONS 10000
#define PBKDF2_SALT_LEN 8

/* CFB is 8-bit feedback and OFB is 128-bit; 3des is des_ede3 (three keys). */
static const char *const cipher_names[CRYPTO_3DES + 1][CRYPTO_CBC + 1] = {
    [CRYPTO_AES128][CRYPTO_ECB] = "aes-128-ecb",
    [CRYPTO_AES128][CRYPTO_CFB] = "aes-128-cfb8",
    [CRYPTO_AES128][CRYPTO_OFB] = "aes-128-ofb",
    [CRYPTO_AES128][CRYPTO_CBC] = "aes-128-cbc",
    [CRYPTO_AES192][CRYPTO_ECB] = "aes-192-ecb",
    [CRYPTO_AES192][CRYPTO_CFB] = "aes-192-cfb8",
    [CRYPTO_AES192][CRYPTO_OFB] = "aes-192-ofb",
    [CRYPTO_AES192][CRYPTO_CBC] = "aes-192-cbc",
    [CRYPTO_AES256][CRYPTO_ECB] = "aes-256-ecb",
    [CRYPTO_AES256][CRYPTO_CFB] = "aes-256-cfb8",
    [CRYPTO_AES256][CRYPTO_OFB] = "aes-256-ofb",
    [CRYPTO_AES256][CRYPTO_CBC] = "aes-256-cbc",
    [CRYPTO_3DES][CRYPTO_ECB] = "des-ede3-ecb",
    [CRYPTO_3DES][CRYPTO_CFB] = "des-ede3-cfb8",
    [CRYPTO_3DES][CRYPTO_OFB] = "des-ede3-ofb",
    [CRYPTO_3DES][CRYPTO_CBC] = "des-ede3-cbc",
};

static const EVP_CIPHER *get_cipher(const crypto_params_t *p)
{
    if (p->algo < CRYPTO_AES128 || p->algo > CRYPTO_3DES)
        return NULL;
    if (p->mode < CRYPTO_ECB || p->mode > CRYPTO_CBC)
        return NULL;
    return EVP_get_cipherbyname(cipher_names[p->algo][p->mode]);
}

/* out gets the key followed by the iv. */
static int derive_key_iv(const char *password, const EVP_CIPHER *cipher, uint8_t *out)
{
    const uint8_t salt[PBKDF2_SALT_LEN] = {0};
    int len = EVP_CIPHER_key_length(cipher) + EVP_CIPHER_iv_length(cipher);

    if (!PKCS5_PBKDF2_HMAC(password, -1, salt, sizeof(salt), PBKDF2_ITERATIONS, EVP_sha256(), len, out))
    {
        fprintf(stderr, "stegobmp: key derivation failed\n");
        return -1;
    }
    return 0;
}

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
