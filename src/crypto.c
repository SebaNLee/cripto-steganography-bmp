#include "crypto.h"

#include <limits.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PBKDF2_ITERATIONS 10000
#define PBKDF2_SALT_LEN 8

static const EVP_CIPHER *get_aes128(crypto_mode_t mode)
{
    switch (mode)
    {
    case CRYPTO_ECB:
        return EVP_aes_128_ecb();
    case CRYPTO_CFB:
        return EVP_aes_128_cfb8();
    case CRYPTO_OFB:
        return EVP_aes_128_ofb();
    case CRYPTO_CBC:
        return EVP_aes_128_cbc();
    default:
        return NULL;
    }
}

static const EVP_CIPHER *get_aes192(crypto_mode_t mode)
{
    switch (mode)
    {
    case CRYPTO_ECB:
        return EVP_aes_192_ecb();
    case CRYPTO_CFB:
        return EVP_aes_192_cfb8();
    case CRYPTO_OFB:
        return EVP_aes_192_ofb();
    case CRYPTO_CBC:
        return EVP_aes_192_cbc();
    default:
        return NULL;
    }
}

static const EVP_CIPHER *get_aes256(crypto_mode_t mode)
{
    switch (mode)
    {
    case CRYPTO_ECB:
        return EVP_aes_256_ecb();
    case CRYPTO_CFB:
        return EVP_aes_256_cfb8();
    case CRYPTO_OFB:
        return EVP_aes_256_ofb();
    case CRYPTO_CBC:
        return EVP_aes_256_cbc();
    default:
        return NULL;
    }
}

/* des_ede3 is three-key 3DES; des_ede would be two-key. */
static const EVP_CIPHER *get_3des(crypto_mode_t mode)
{
    switch (mode)
    {
    case CRYPTO_ECB:
        return EVP_des_ede3_ecb();
    case CRYPTO_CFB:
        return EVP_des_ede3_cfb8();
    case CRYPTO_OFB:
        return EVP_des_ede3_ofb();
    case CRYPTO_CBC:
        return EVP_des_ede3_cbc();
    default:
        return NULL;
    }
}

/* CFB is 8-bit feedback (cfb8) and OFB is 128-bit. */
static const EVP_CIPHER *get_cipher(const crypto_params_t *p)
{
    switch (p->algo)
    {
    case CRYPTO_AES128:
        return get_aes128(p->mode);
    case CRYPTO_AES192:
        return get_aes192(p->mode);
    case CRYPTO_AES256:
        return get_aes256(p->mode);
    case CRYPTO_3DES:
        return get_3des(p->mode);
    default:
        return NULL;
    }
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

/* enc is 1 to encrypt and 0 to decrypt, as in EVP_CipherInit_ex. */
static int run_cipher(const crypto_params_t *p, int enc, const uint8_t *in, size_t in_len,
                      uint8_t **out, size_t *out_len)
{
    const char *action = enc ? "encryption" : "decryption";
    uint8_t key_iv[EVP_MAX_KEY_LENGTH + EVP_MAX_IV_LENGTH];
    EVP_CIPHER_CTX *ctx = NULL;
    uint8_t *buf = NULL;
    int update_len = 0;
    int final_len = 0;
    int ret = -1;

    const EVP_CIPHER *cipher = get_cipher(p);
    if (cipher == NULL)
    {
        fprintf(stderr, "stegobmp: invalid algorithm or mode\n");
        return -1;
    }
    /* EVP_CipherUpdate takes an int length. */
    if (in_len > INT_MAX - EVP_MAX_BLOCK_LENGTH)
    {
        fprintf(stderr, "stegobmp: data too large for %s\n", action);
        return -1;
    }
    if (derive_key_iv(p->password, cipher, key_iv) != 0)
        return -1;

    buf = malloc(in_len + EVP_CIPHER_block_size(cipher));
    ctx = EVP_CIPHER_CTX_new();
    if (buf == NULL || ctx == NULL)
    {
        fprintf(stderr, "stegobmp: out of memory\n");
        goto cleanup;
    }

    const uint8_t *key = key_iv;
    const uint8_t *iv = key_iv + EVP_CIPHER_key_length(cipher);
    if (!EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, enc) ||
        !EVP_CipherUpdate(ctx, buf, &update_len, in, (int)in_len) ||
        !EVP_CipherFinal_ex(ctx, buf + update_len, &final_len))
    {
        fprintf(stderr, "stegobmp: %s failed\n", action);
        goto cleanup;
    }

    *out = buf;
    *out_len = (size_t)update_len + (size_t)final_len;
    buf = NULL;
    ret = 0;

cleanup:
    OPENSSL_cleanse(key_iv, sizeof(key_iv));
    EVP_CIPHER_CTX_free(ctx);
    free(buf);
    return ret;
}

int crypto_encrypt(const crypto_params_t *p, const uint8_t *in, size_t in_len, uint8_t **out,
                   size_t *out_len)
{
    return run_cipher(p, 1, in, in_len, out, out_len);
}

int crypto_decrypt(const crypto_params_t *p, const uint8_t *in, size_t in_len, uint8_t **out,
                   size_t *out_len)
{
    return run_cipher(p, 0, in, in_len, out, out_len);
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
