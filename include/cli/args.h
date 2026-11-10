#ifndef ARGS_H
#define ARGS_H

// Operaciones que puede realizar el programa
typedef enum {
    OP_EMBED,
    OP_EXTRACT
} operation_t;

// Argumentos introducidos por el usuario
typedef struct {
    operation_t operation;

    const char *input_file;
    const char *carrier_file;
    const char *output_file;

    const char *steg_algorithm;

    const char *crypto_algorithm;
    const char *crypto_mode;
    const char *password;

} args_t;

// Lee y valida los argumentos de la terminal
int args_parse(int argc, const char *argv[], args_t *args);

#endif
