#include "lib/analyze.h"
#include "lib/bit.h"

#include <stdlib.h>
#include <string.h>

#define N_ALGOS 4
#define N_MODES 4

static const CryptoAlgo ALGOS[N_ALGOS] = {CRYPTO_AES128, CRYPTO_AES192, CRYPTO_AES256, CRYPTO_3DES};
static const CryptoMode MODES[N_MODES] = {CRYPTO_ECB, CRYPTO_CFB, CRYPTO_OFB, CRYPTO_CBC};

static int try_decrypt(const uint8_t *blob, size_t blob_len, const CryptoParams *given, AnalyzeFinding *finding)
{
    for (int i = 0; i < N_ALGOS; i++)
    {
        if (given->algo != CRYPTO_ALGO_UNSET && given->algo != ALGOS[i])
        {
            continue;
        }

        for (int j = 0; j < N_MODES; j++)
        {
            if (given->mode != CRYPTO_MODE_UNSET && given->mode != MODES[j])
            {
                continue;
            }

            CryptoParams candidate = {ALGOS[i], MODES[j], given->password};

            if (payload_decode(blob, blob_len, &candidate, &finding->payload) == PAYLOAD_OK)
            {
                finding->crypto = candidate;
                return 1;
            }
        }
    }

    return 0;
}

AnalyzeResult analyze_algorithm(StegAlgo algo, const Bmp *bmp, const CryptoParams *crypto, AnalyzeFinding *finding)
{
    CryptoParams no_crypto = {0};
    uint8_t head[PAYLOAD_HEADER_SIZE];
    uint8_t *blob;
    size_t capacity;
    size_t declared;
    size_t ext_room;
    size_t blob_len;

    if (bmp == NULL || crypto == NULL || finding == NULL)
    {
        return ANALYZE_ERR_INVALID_ARG;
    }

    memset(finding, 0, sizeof(*finding));

    capacity = steg_capacity(algo, bmp->body_len);
    if (capacity < PAYLOAD_HEADER_SIZE)
    {
        return ANALYZE_OK;
    }

    if (steg_extract(algo, bmp->body, bmp->body_len, head, sizeof(head)) != STEG_OK)
    {
        return ANALYZE_ERR_STEG;
    }

    // the size comes from the carrier, so a size that doesn't fit means there is nothing hidden
    declared = be32_read(head);
    if (declared == 0 || declared > capacity - PAYLOAD_HEADER_SIZE)
    {
        return ANALYZE_OK;
    }

    // without encryption the extension comes after the data and has no length, so read some extra
    ext_room = capacity - PAYLOAD_HEADER_SIZE - declared;
    if (ext_room > PAYLOAD_EXT_MAX)
    {
        ext_room = PAYLOAD_EXT_MAX;
    }

    blob_len = PAYLOAD_HEADER_SIZE + declared + ext_room;
    blob = malloc(blob_len);
    if (blob == NULL)
    {
        return ANALYZE_ERR_NO_MEMORY;
    }

    if (steg_extract(algo, bmp->body, bmp->body_len, blob, blob_len) != STEG_OK)
    {
        free(blob);
        return ANALYZE_ERR_STEG;
    }

    finding->declared_len = declared;

    if (payload_decode(blob, blob_len, &no_crypto, &finding->payload) == PAYLOAD_OK)
    {
        finding->status = ANALYZE_PLAIN;
    }
    else if (crypto->password == NULL)
    {
        finding->status = ANALYZE_NEEDS_PASSWORD;
    }
    else if (try_decrypt(blob, PAYLOAD_HEADER_SIZE + declared, crypto, finding))
    {
        finding->status = ANALYZE_DECRYPTED;
    }
    else
    {
        finding->status = ANALYZE_NO_MATCH;
    }

    free(blob);

    return ANALYZE_OK;
}

static int trailing_is_printable(const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        unsigned char c = data[i];

        if ((c >= 0x20 && c <= 0x7e) || c == '\r' || c == '\n' || c == '\t')
        {
            continue;
        }

        return 0;
    }

    return 1;
}

AnalyzeResult analyze_trailing(const Bmp *bmp, AnalyzeTrailing *trailing)
{
    if (bmp == NULL || trailing == NULL)
    {
        return ANALYZE_ERR_INVALID_ARG;
    }

    memset(trailing, 0, sizeof(*trailing));

    if (!bmp->declared_size_valid || bmp->trailing_len == 0)
    {
        return ANALYZE_OK;
    }

    if (bmp->trailing_offset > bmp->size ||
        bmp->trailing_len > bmp->size - bmp->trailing_offset)
    {
        return ANALYZE_ERR_INVALID_ARG;
    }

    trailing->present = 1;
    trailing->offset = bmp->trailing_offset;
    trailing->length = bmp->trailing_len;
    trailing->printable = trailing_is_printable(bmp->data + bmp->trailing_offset, bmp->trailing_len);

    return ANALYZE_OK;
}

void analyze_finding_free(AnalyzeFinding *finding)
{
    if (finding == NULL)
    {
        return;
    }

    payload_free(&finding->payload);
}

const char *analyze_error_string(AnalyzeResult result)
{
    switch (result)
    {
    case ANALYZE_OK:
        return "ok";
    case ANALYZE_ERR_INVALID_ARG:
        return "invalid analyze argument";
    case ANALYZE_ERR_NO_MEMORY:
        return "out of memory";
    case ANALYZE_ERR_STEG:
        return "could not read the hidden data from the carrier";
    default:
        return "unknown error";
    }
}
