#include "cli/args.h"

#include <stddef.h>
#include <string.h>

// Sintaxis del programa
#define ARGS_USAGE                                                            \
    "Usage:\n"                                                                \
    "  stegobmp -embed -in <file> -p <bitmap.bmp> -out <bitmap.bmp> "         \
    "-steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] "                \
    "[-m <ecb|cfb|ofb|cbc>] [-pass <password>]\n"                             \
    "  stegobmp -extract -p <bitmap.bmp> -out <file> -steg <LSB1|LSB4|LSBI> " \
    "[-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] "                \
    "[-pass <password>]\n"

// Comprueba que una opcion tenga un valor valido
static int valid_value(const char *value)
{
    return value != NULL && value[0] != '\0' && value[0] != '-';
}

// Lee y valida los argumentos introducidos por el usuario
args_result_t args_parse(int argc, const char *argv[], args_t *args)
{
    // Comprobamos que los argumentos existan
    if (args == NULL || argv == NULL || argc < 2 || argv[1] == NULL)
    {
        return ARGS_ERR_OPERATION;
    }

    // Inicializamos todos los campos
    memset(args, 0, sizeof(*args));

    args->crypto.algo = CRYPTO_ALGO_UNSET;
    args->crypto.mode = CRYPTO_MODE_UNSET;

    // Comprobamos la operacion
    if (strcmp(argv[1], "-embed") == 0)
    {
        args->operation = OP_EMBED;
    }
    else if (strcmp(argv[1], "-extract") == 0)
    {
        args->operation = OP_EXTRACT;
    }
    else
    {
        return ARGS_ERR_OPERATION;
    }

    // Controlamos si una opcion ya fue indicada
    int has_in = 0;
    int has_p = 0;
    int has_out = 0;
    int has_steg = 0;
    int has_algo = 0;
    int has_mode = 0;
    int has_pass = 0;

    // Recorremos los argumentos
    for (int i = 2; i < argc; i++)
    {

        // Cada opcion debe tener un valor
        if (argv[i] == NULL || i + 1 >= argc)
        {
            return ARGS_ERR_MISSING_VALUE;
        }

        if (!valid_value(argv[i + 1]))
        {
            return ARGS_ERR_INVALID_VALUE;
        }

        const char *option = argv[i];
        const char *value = argv[++i];

        // Archivo que queremos ocultar
        if (strcmp(option, "-in") == 0)
        {
            if (has_in)
            {
                return ARGS_ERR_INVALID_VALUE;
            }
            if (args->operation != OP_EMBED)
            {
                return ARGS_ERR_OPERATION;
            }
            args->input_file = value;
            has_in = 1;
        }

        // Imagen BMP portadora
        else if (strcmp(option, "-p") == 0)
        {
            if (has_p)
            {
                return ARGS_ERR_INVALID_VALUE;
            }
            args->carrier_file = value;
            has_p = 1;
        }

        // Archivo de salida
        else if (strcmp(option, "-out") == 0)
        {
            if (has_out)
            {
                return ARGS_ERR_INVALID_VALUE;
            }
            args->output_file = value;
            has_out = 1;
        }

        // Algoritmo de esteganografia
        else if (strcmp(option, "-steg") == 0)
        {
            if (has_steg)
            {
                return ARGS_ERR_INVALID_VALUE;
            }

            if (strcmp(value, "LSB1") == 0)
            {
                args->steg_algorithm = LSB1;
            }
            else if (strcmp(value, "LSB4") == 0)
            {
                args->steg_algorithm = LSB4;
            }
            else if (strcmp(value, "LSBI") == 0)
            {
                args->steg_algorithm = LSBI;
            }
            else
            {
                return ARGS_ERR_INVALID_VALUE;
            }

            has_steg = 1;
        }

        // Algoritmo de cifrado
        else if (strcmp(option, "-a") == 0)
        {
            if (has_algo)
            {
                return ARGS_ERR_INVALID_VALUE;
            }

            args->crypto.algo = crypto_algo_from_str(value);

            if (args->crypto.algo == CRYPTO_ALGO_INVALID ||
                args->crypto.algo == CRYPTO_ALGO_UNSET)
            {
                return ARGS_ERR_INVALID_VALUE;
            }

            has_algo = 1;
        }

        // Modo de cifrado
        else if (strcmp(option, "-m") == 0)
        {
            if (has_mode)
            {
                return ARGS_ERR_INVALID_VALUE;
            }

            args->crypto.mode = crypto_mode_from_str(value);

            if (args->crypto.mode == CRYPTO_MODE_INVALID ||
                args->crypto.mode == CRYPTO_MODE_UNSET)
            {
                return ARGS_ERR_INVALID_VALUE;
            }

            has_mode = 1;
        }

        // Contrasena de cifrado
        else if (strcmp(option, "-pass") == 0)
        {
            if (has_pass)
            {
                return ARGS_ERR_INVALID_VALUE;
            }

            args->crypto.password = value;
            has_pass = 1;
        }

        // Opcion desconocida
        else
        {
            return ARGS_ERR_UNKNOWN_OPTION;
        }
    }

    // Comprobamos los argumentos obligatorios
    if (!has_p || !has_out || !has_steg)
    {
        return ARGS_ERR_MISSING_REQUIRED;
    }

    // Para ocultar un archivo necesitamos -in
    if (args->operation == OP_EMBED && !has_in)
    {
        return ARGS_ERR_MISSING_REQUIRED;
    }

    // Aplicamos los valores por defecto del cifrado
    // Solo se cifra cuando hay contrasena
    if (has_pass)
    {
        crypto_apply_defaults(&args->crypto);
    }

    // Argumentos correctos
    return ARGS_OK;
}

// Devuelve el mensaje de error junto con la sintaxis
const char *args_error_string(args_result_t result)
{
    switch (result)
    {
    case ARGS_OK:
        return "ok";
    case ARGS_ERR_OPERATION:
        return "expected exactly one of -embed or -extract, and -in only with -embed\n" ARGS_USAGE;
    case ARGS_ERR_UNKNOWN_OPTION:
        return "unknown option\n" ARGS_USAGE;
    case ARGS_ERR_MISSING_VALUE:
        return "option requires a value\n" ARGS_USAGE;
    case ARGS_ERR_INVALID_VALUE:
        return "invalid or duplicate option value\n" ARGS_USAGE;
    case ARGS_ERR_MISSING_REQUIRED:
        return "missing required option (-in, -p, -out or -steg)\n" ARGS_USAGE;
    default:
        return "unknown error\n" ARGS_USAGE;
    }
}
