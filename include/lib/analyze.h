#ifndef ANALYZE_H
#define ANALYZE_H

#include "enc/crypto.h"
#include "lib/bmp.h"
#include "lib/payload.h"
#include "steg/steg.h"

typedef enum
{
    ANALYZE_NOTHING = 0,
    ANALYZE_PLAIN,
    ANALYZE_NEEDS_PASSWORD,
    ANALYZE_DECRYPTED,
    ANALYZE_NO_MATCH
} AnalyzeStatus;

typedef enum
{
    ANALYZE_OK = 0,
    ANALYZE_ERR_INVALID_ARG,
    ANALYZE_ERR_NO_MEMORY,
    ANALYZE_ERR_STEG
} AnalyzeResult;

typedef struct
{
    AnalyzeStatus status;
    size_t declared_len;
    CryptoParams crypto;
    Payload payload;
} AnalyzeFinding;

// algo and mode in crypto may be UNSET, and then every algorithm and mode is tried
AnalyzeResult analyze_algorithm(StegAlgo algo, const Bmp *bmp, const CryptoParams *crypto, AnalyzeFinding *finding);

void analyze_finding_free(AnalyzeFinding *finding);

const char *analyze_error_string(AnalyzeResult result);

#endif
