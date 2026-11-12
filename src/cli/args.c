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

static args_result_t fail(args_t *args, args_result_t result, const char *option, const char *value)
{
    args->bad_option = option;
    args->bad_value = value;

    return result;
}

// Lee y valida los argumentos introducidos por el usuario
args_result_t args_parse(int argc, const char *argv[], args_t *args)
{
    if (args == NULL)
    {
        return ARGS_ERR_OPERATION;
    }

    // Inicializamos todos los campos
    memset(args, 0, sizeof(*args));

    if (argv == NULL || argc < 2 || argv[1] == NULL)
    {
        return ARGS_ERR_OPERATION;
    }

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
        return fail(args, ARGS_ERR_OPERATION, argv[1], NULL);
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
            return fail(args, ARGS_ERR_MISSING_VALUE, argv[i], NULL);
        }

        if (!valid_value(argv[i + 1]))
        {
            return fail(args, ARGS_ERR_INVALID_VALUE, argv[i], argv[i + 1]);
        }

        const char *option = argv[i];
        const char *value = argv[++i];

        // Archivo que queremos ocultar
        if (strcmp(option, "-in") == 0)
        {
            if (has_in)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }
            if (args->operation != OP_EMBED)
            {
                return fail(args, ARGS_ERR_OPERATION, option, NULL);
            }
            args->input_file = value;
            has_in = 1;
        }

        // Imagen BMP portadora
        else if (strcmp(option, "-p") == 0)
        {
            if (has_p)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }
            args->carrier_file = value;
            has_p = 1;
        }

        // Archivo de salida
        else if (strcmp(option, "-out") == 0)
        {
            if (has_out)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }
            args->output_file = value;
            has_out = 1;
        }

        // Algoritmo de esteganografia
        else if (strcmp(option, "-steg") == 0)
        {
            if (has_steg)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
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
                return fail(args, ARGS_ERR_INVALID_VALUE, option, value);
            }

            has_steg = 1;
        }

        // Algoritmo de cifrado
        else if (strcmp(option, "-a") == 0)
        {
            if (has_algo)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }

            args->crypto.algo = crypto_algo_from_str(value);

            if (args->crypto.algo == CRYPTO_ALGO_INVALID ||
                args->crypto.algo == CRYPTO_ALGO_UNSET)
            {
                return fail(args, ARGS_ERR_INVALID_VALUE, option, value);
            }

            has_algo = 1;
        }

        // Modo de cifrado
        else if (strcmp(option, "-m") == 0)
        {
            if (has_mode)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }

            args->crypto.mode = crypto_mode_from_str(value);

            if (args->crypto.mode == CRYPTO_MODE_INVALID ||
                args->crypto.mode == CRYPTO_MODE_UNSET)
            {
                return fail(args, ARGS_ERR_INVALID_VALUE, option, value);
            }

            has_mode = 1;
        }

        // Contrasena de cifrado
        else if (strcmp(option, "-pass") == 0)
        {
            if (has_pass)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }

            args->crypto.password = value;
            has_pass = 1;
        }

        // Opcion desconocida
        else
        {
            return fail(args, ARGS_ERR_UNKNOWN_OPTION, option, NULL);
        }
    }

    // Comprobamos los argumentos obligatorios
    if (!has_p)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-p", NULL);
    }
    if (!has_out)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-out", NULL);
    }
    if (!has_steg)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-steg", NULL);
    }

    // Para ocultar un archivo necesitamos -in
    if (args->operation == OP_EMBED && !has_in)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-in", NULL);
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

const char *args_error_string(args_result_t result)
{
    switch (result)
    {
    case ARGS_OK:
        return "ok";
    case ARGS_ERR_OPERATION:
        return "expected exactly one of -embed or -extract, and -in only with -embed";
    case ARGS_ERR_UNKNOWN_OPTION:
        return "unknown option";
    case ARGS_ERR_MISSING_VALUE:
        return "option requires a value";
    case ARGS_ERR_INVALID_VALUE:
        return "invalid option value";
    case ARGS_ERR_DUPLICATE_OPTION:
        return "option given more than once";
    case ARGS_ERR_MISSING_REQUIRED:
        return "missing required option";
    default:
        return "unknown error";
    }
}

const char *args_usage(void)
{
    return ARGS_USAGE;
}
