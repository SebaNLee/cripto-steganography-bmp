
#ifndef ARGS_H
#define ARGS_H

#include "enc/crypto.h"
#include "steg/steg.h"

// Operaciones disponibles
typedef enum
{
    OP_EMBED,
    OP_EXTRACT
} operation_t;

typedef enum
{
    ARGS_OK = 0,
    ARGS_ERR_OPERATION,
    ARGS_ERR_UNKNOWN_OPTION,
    ARGS_ERR_MISSING_VALUE,
    ARGS_ERR_INVALID_VALUE,
    ARGS_ERR_DUPLICATE_OPTION,
    ARGS_ERR_MISSING_REQUIRED
} args_result_t;

// Argumentos del programa
typedef struct
{
    operation_t operation;

    const char *input_file;
    const char *carrier_file;
    const char *output_file;

    StegAlgo steg_algorithm;
    CryptoParams crypto;

    const char *bad_option;
    const char *bad_value;

} args_t;

args_result_t args_parse(int argc, const char *argv[], args_t *args);

const char *args_error_string(args_result_t result);

const char *args_usage(void);

#endif
