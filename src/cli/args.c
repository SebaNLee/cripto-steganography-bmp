#include "cli/args.h"

#include <stddef.h>
#include <string.h>

#define ARGS_USAGE                                                            \
    "Usage:\n"                                                                \
    "  stegobmp -embed -in <file> -p <bitmap.bmp> -out <bitmap.bmp> "         \
    "-steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] "                \
    "[-m <ecb|cfb|ofb|cbc>] [-pass <password>]\n"                             \
    "  stegobmp -extract -p <bitmap.bmp> -out <file> -steg <LSB1|LSB4|LSBI> " \
    "[-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] "                \
    "[-pass <password>]\n"                                                    \
    "  stegobmp -analyze -p <bitmap.bmp> [-out <file>] "                      \
    "[-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] "                \
    "[-pass <password>]\n"

static int valid_value(const char *value)
{
    return value != NULL && value[0] != '\0' && value[0] != '-';
}

static ArgsResult fail(Args *args, ArgsResult result, const char *option, const char *value)
{
    args->bad_option = option;
    args->bad_value = value;

    return result;
}

ArgsResult args_parse(int argc, const char *argv[], Args *args)
{
    if (args == NULL)
    {
        return ARGS_ERR_OPERATION;
    }

    memset(args, 0, sizeof(*args));

    if (argv == NULL || argc < 2 || argv[1] == NULL)
    {
        return ARGS_ERR_OPERATION;
    }

    args->crypto.algo = CRYPTO_ALGO_UNSET;
    args->crypto.mode = CRYPTO_MODE_UNSET;

    if (strcmp(argv[1], "-embed") == 0)
    {
        args->operation = OP_EMBED;
    }
    else if (strcmp(argv[1], "-extract") == 0)
    {
        args->operation = OP_EXTRACT;
    }
    else if (strcmp(argv[1], "-analyze") == 0)
    {
        args->operation = OP_ANALYZE;
    }
    else
    {
        return fail(args, ARGS_ERR_OPERATION, argv[1], NULL);
    }

    int has_in = 0;
    int has_p = 0;
    int has_out = 0;
    int has_steg = 0;
    int has_algo = 0;
    int has_mode = 0;
    int has_pass = 0;

    for (int i = 2; i < argc; i++)
    {
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

        if (strcmp(option, "-in") == 0)
        {
            if (has_in)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }
            if (args->operation != OP_EMBED)
            {
                return fail(args, ARGS_ERR_NOT_ALLOWED, option, NULL);
            }
            args->input_file = value;
            has_in = 1;
        }
        else if (strcmp(option, "-p") == 0)
        {
            if (has_p)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }
            args->carrier_file = value;
            has_p = 1;
        }
        else if (strcmp(option, "-out") == 0)
        {
            if (has_out)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }
            args->output_file = value;
            has_out = 1;
        }
        else if (strcmp(option, "-steg") == 0)
        {
            if (has_steg)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }
            if (args->operation == OP_ANALYZE)
            {
                return fail(args, ARGS_ERR_NOT_ALLOWED, option, NULL);
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
        else if (strcmp(option, "-pass") == 0)
        {
            if (has_pass)
            {
                return fail(args, ARGS_ERR_DUPLICATE_OPTION, option, NULL);
            }

            args->crypto.password = value;
            has_pass = 1;
        }
        else
        {
            return fail(args, ARGS_ERR_UNKNOWN_OPTION, option, NULL);
        }
    }

    if (!has_p)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-p", NULL);
    }
    if (args->operation != OP_ANALYZE && !has_out)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-out", NULL);
    }
    if (args->operation != OP_ANALYZE && !has_steg)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-steg", NULL);
    }

    if (args->operation == OP_EMBED && !has_in)
    {
        return fail(args, ARGS_ERR_MISSING_REQUIRED, "-in", NULL);
    }

    // analyze leaves algorithm and mode unset so it can try all of them
    if (has_pass && args->operation != OP_ANALYZE)
    {
        crypto_apply_defaults(&args->crypto);
    }

    return ARGS_OK;
}

const char *args_error_string(ArgsResult result)
{
    switch (result)
    {
    case ARGS_OK:
        return "ok";
    case ARGS_ERR_OPERATION:
        return "expected -embed, -extract or -analyze as the first argument";
    case ARGS_ERR_UNKNOWN_OPTION:
        return "unknown option";
    case ARGS_ERR_MISSING_VALUE:
        return "option requires a value";
    case ARGS_ERR_INVALID_VALUE:
        return "invalid option value";
    case ARGS_ERR_DUPLICATE_OPTION:
        return "option given more than once";
    case ARGS_ERR_NOT_ALLOWED:
        return "option not allowed with this operation";
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
