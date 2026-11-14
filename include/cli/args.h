#ifndef ARGS_H
#define ARGS_H

#include "enc/crypto.h"
#include "steg/steg.h"

typedef enum
{
    OP_EMBED,
    OP_EXTRACT,
    OP_ANALYZE
} Operation;

typedef enum
{
    ARGS_OK = 0,
    ARGS_ERR_OPERATION,
    ARGS_ERR_UNKNOWN_OPTION,
    ARGS_ERR_MISSING_VALUE,
    ARGS_ERR_INVALID_VALUE,
    ARGS_ERR_DUPLICATE_OPTION,
    ARGS_ERR_NOT_ALLOWED,
    ARGS_ERR_MISSING_REQUIRED,
    ARGS_ERR_CRYPTO_WITHOUT_PASSWORD
} ArgsResult;

typedef struct
{
    Operation operation;

    const char *input_file;
    const char *carrier_file;
    const char *output_file;

    StegAlgo steg_algorithm;
    CryptoParams crypto;

    const char *bad_option;
    const char *bad_value;

} Args;

ArgsResult args_parse(int argc, const char *argv[], Args *args);

const char *args_error_string(ArgsResult result);

const char *args_usage(void);

#endif
