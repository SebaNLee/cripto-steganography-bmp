#include "lib/payload.h"
#include "lib/bit.h"

#include <stdlib.h>
#include <string.h>

#define PAYLOAD_HEADER_SIZE 4

// sin cifrado: BE32(tamaño real) || datos archivo || extensión
// con cifrado: BE32(tamaño cifrado) || encriptación(tamaño real || datos archivo || extensión)

static int payload_crypto_enabled(const CryptoParams *crypto)
{
    return crypto != NULL && crypto_enabled(crypto);
}

static PayloadResult payload_extension_from_path(const char *path, char *extension)
{
    const char *slash;
    const char *backslash;
    const char *base;
    const char *dot;
    size_t len;

    if (path == NULL || extension == NULL)
    {
        return PAYLOAD_ERR_INVALID;
    }

    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');

    if (backslash != NULL && (slash == NULL || backslash > slash))
    {
        slash = backslash;
    }

    base = slash == NULL ? path : slash + 1;
    dot = strrchr(base, '.');

    if (dot == NULL || dot == base || dot[1] == '\0')
    {
        return PAYLOAD_ERR_INVALID;
    }

    len = strlen(dot);

    if (len + 1 > PAYLOAD_EXT_MAX)
    {
        return PAYLOAD_ERR_INVALID;
    }

    memcpy(extension, dot, len + 1);
    return PAYLOAD_OK;
}

static PayloadResult payload_parse_plain(const uint8_t *plain, size_t plain_len, Payload *payload)
{
    uint32_t declared;
    size_t data_len;
    size_t ext_offset;
    size_t ext_available;
    size_t ext_len;
    const uint8_t *nul;

    if (plain == NULL || payload == NULL)
    {
        return PAYLOAD_ERR_INVALID;
    }

    if (plain_len < PAYLOAD_HEADER_SIZE)
    {
        return PAYLOAD_ERR_INVALID;
    }

    // formato plano: BE32(tamaño real) || datos archivo || extensión
    declared = be32_read(plain);
    data_len = (size_t)declared;

    if (data_len > plain_len - PAYLOAD_HEADER_SIZE)
    {
        return PAYLOAD_ERR_INVALID;
    }

    ext_offset = PAYLOAD_HEADER_SIZE + data_len;
    ext_available = plain_len - ext_offset;

    if (ext_available > PAYLOAD_EXT_MAX)
    {
        ext_available = PAYLOAD_EXT_MAX;
    }

    nul = memchr(plain + ext_offset, '\0', ext_available);
    if (nul == NULL)
    {
        return PAYLOAD_ERR_INVALID;
    }

    ext_len = (size_t)(nul - (plain + ext_offset));

    if (ext_len < 0x02 || plain[ext_offset] != '.')
    {
        return PAYLOAD_ERR_INVALID;
    }

    for (size_t i = 0; i < ext_len; i++)
    {
        char c = (char)plain[ext_offset + i];

        if (c == '/' || c == '\\')
        {
            return PAYLOAD_ERR_INVALID;
        }
    }

    payload->data = malloc(data_len == 0 ? 1 : data_len);
    if (payload->data == NULL)
    {
        return PAYLOAD_ERR_NO_MEMORY;
    }

    if (data_len > 0)
    {
        memcpy(payload->data, plain + PAYLOAD_HEADER_SIZE, data_len);
    }

    payload->data_len = data_len;
    memcpy(payload->extension, plain + ext_offset, ext_len);
    payload->extension[ext_len] = '\0';

    return PAYLOAD_OK;
}

PayloadResult payload_encode(const uint8_t *data, size_t data_len, const char *input_path, const CryptoParams *crypto, uint8_t **out, size_t *out_len, CryptoResult *crypto_result)
{
    char extension[PAYLOAD_EXT_MAX];
    uint8_t *plain = NULL;
    uint8_t *cipher = NULL;
    uint8_t *blob = NULL;
    size_t ext_len;
    size_t plain_len;
    size_t cipher_len = 0;
    CryptoResult crypto_res;
    PayloadResult result;

    if (crypto_result != NULL)
    {
        *crypto_result = CRYPTO_OK;
    }

    if (out == NULL || out_len == NULL || (data_len > 0 && data == NULL))
    {
        return PAYLOAD_ERR_INVALID;
    }

    *out = NULL;
    *out_len = 0;

    result = payload_extension_from_path(input_path, extension);
    if (result != PAYLOAD_OK)
    {
        return result;
    }

    if (data_len > UINT32_MAX)
    {
        return PAYLOAD_ERR_INVALID;
    }

    ext_len = strlen(extension) + 1;

    if (ext_len > SIZE_MAX - PAYLOAD_HEADER_SIZE ||
        data_len > SIZE_MAX - PAYLOAD_HEADER_SIZE - ext_len)
    {
        return PAYLOAD_ERR_INVALID;
    }

    // sin cifrado: BE32(tamaño real) || datos archivo || extensión
    plain_len = PAYLOAD_HEADER_SIZE + data_len + ext_len;
    plain = malloc(plain_len);

    if (plain == NULL)
    {
        return PAYLOAD_ERR_NO_MEMORY;
    }

    be32_write(plain, (uint32_t)data_len);

    if (data_len > 0)
    {
        memcpy(plain + PAYLOAD_HEADER_SIZE, data, data_len);
    }

    memcpy(plain + PAYLOAD_HEADER_SIZE + data_len, extension, ext_len);

    if (!payload_crypto_enabled(crypto))
    {
        *out = plain;
        *out_len = plain_len;
        return PAYLOAD_OK;
    }

    // con cifrado: BE32(tamaño cifrado) || encriptación(bloque plano)
    crypto_res = crypto_encrypt(crypto, plain, plain_len, &cipher, &cipher_len);
    free(plain);

    if (crypto_res != CRYPTO_OK)
    {
        if (crypto_result != NULL)
        {
            *crypto_result = crypto_res;
        }

        return PAYLOAD_ERR_CRYPTO;
    }

    if (cipher_len > UINT32_MAX || cipher_len > SIZE_MAX - PAYLOAD_HEADER_SIZE)
    {
        free(cipher);
        return PAYLOAD_ERR_INVALID;
    }

    blob = malloc(PAYLOAD_HEADER_SIZE + cipher_len);
    if (blob == NULL)
    {
        free(cipher);
        return PAYLOAD_ERR_NO_MEMORY;
    }

    be32_write(blob, (uint32_t)cipher_len);
    memcpy(blob + PAYLOAD_HEADER_SIZE, cipher, cipher_len);
    free(cipher);

    *out = blob;
    *out_len = PAYLOAD_HEADER_SIZE + cipher_len;

    return PAYLOAD_OK;
}

PayloadResult payload_decode(const uint8_t *blob, size_t blob_len, const CryptoParams *crypto, Payload *payload, CryptoResult *crypto_result)
{
    uint8_t *plain = NULL;
    size_t plain_len = 0;
    uint32_t declared;
    CryptoResult crypto_res;
    PayloadResult result;

    if (crypto_result != NULL)
    {
        *crypto_result = CRYPTO_OK;
    }

    if (payload == NULL)
    {
        return PAYLOAD_ERR_INVALID;
    }

    memset(payload, 0, sizeof(*payload));

    if (blob == NULL || blob_len < PAYLOAD_HEADER_SIZE)
    {
        return PAYLOAD_ERR_INVALID;
    }

    declared = be32_read(blob);

    if (!payload_crypto_enabled(crypto))
    {
        // sin cifrado: BE32(tamaño real) || datos archivo || extensión
        return payload_parse_plain(blob, blob_len, payload);
    }

    // con cifrado: BE32(tamaño cifrado) || encriptación(tamaño real || datos || extensión)
    if ((size_t)declared > blob_len - PAYLOAD_HEADER_SIZE)
    {
        return PAYLOAD_ERR_INVALID;
    }

    crypto_res = crypto_decrypt(crypto, blob + PAYLOAD_HEADER_SIZE, (size_t)declared, &plain,
                                &plain_len);
    if (crypto_res != CRYPTO_OK)
    {
        if (crypto_result != NULL)
        {
            *crypto_result = crypto_res;
        }

        return PAYLOAD_ERR_CRYPTO;
    }

    result = payload_parse_plain(plain, plain_len, payload);
    free(plain);

    return result;
}

void payload_free(Payload *payload)
{
    if (payload == NULL)
    {
        return;
    }

    free(payload->data);
    memset(payload, 0, sizeof(*payload));
}

const char *payload_error_string(PayloadResult result)
{
    switch (result)
    {
    case PAYLOAD_OK:
        return "ok";
    case PAYLOAD_ERR_INVALID:
        return "invalid or malformed payload";
    case PAYLOAD_ERR_NO_MEMORY:
        return "out of memory";
    case PAYLOAD_ERR_CRYPTO:
        return "encryption or decryption failed";
    default:
        return "unknown error";
    }
}
