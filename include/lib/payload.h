#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "enc/crypto.h"

#include <stddef.h>
#include <stdint.h>

#define PAYLOAD_EXT_MAX 256

typedef enum
{
    PAYLOAD_OK = 0,
    PAYLOAD_ERR_INVALID,
    PAYLOAD_ERR_NO_MEMORY,
    PAYLOAD_ERR_CRYPTO
} PayloadResult;

typedef struct
{
    uint8_t *data;
    size_t data_len;
    char extension[PAYLOAD_EXT_MAX];
} Payload;

PayloadResult payload_encode(const uint8_t *data, size_t data_len, const char *input_path, const CryptoParams *crypto, uint8_t **out, size_t *out_len, CryptoResult *crypto_result);

PayloadResult payload_decode(const uint8_t *blob, size_t blob_len, const CryptoParams *crypto, Payload *payload, CryptoResult *crypto_result);

void payload_free(Payload *payload);

const char *payload_error_string(PayloadResult result);

#endif
