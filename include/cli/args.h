
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

// Argumentos del programa
typedef struct
{
    operation_t operation;

    const char *input_file;
    const char *carrier_file;
    const char *output_file;

    StegAlgo steg_algorithm;
    CryptoParams crypto;

} args_t;

// Lee y valida los argumentos de la terminal.
// Devuelve 0 si son correctos y -1 si hay un error.
int args_parse(int argc, const char *argv[], args_t *args);

#endif
