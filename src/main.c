#include "cli/args.h"
#include "lib/analyze.h"
#include "lib/bmp.h"
#include "lib/file.h"
#include "lib/payload.h"
#include "steg/steg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *path_with_extension(const char *path, const char *extension)
{
    size_t path_len = strlen(path);
    size_t ext_len = strlen(extension);
    char *result;

    if (path_len >= ext_len && strcmp(path + path_len - ext_len, extension) == 0)
    {
        result = malloc(path_len + 1);
        if (result != NULL)
        {
            memcpy(result, path, path_len + 1);
        }

        return result;
    }

    result = malloc(path_len + ext_len + 1);
    if (result == NULL)
    {
        return NULL;
    }

    memcpy(result, path, path_len);
    memcpy(result + path_len, extension, ext_len + 1);

    return result;
}

static int run_embed(const Args *args)
{
    uint8_t *secret = NULL;
    size_t secret_len = 0;
    uint8_t *blob = NULL;
    size_t blob_len = 0;
    uint8_t *bmp_data = NULL;
    size_t bmp_size = 0;
    Bmp bmp;
    FileResult file_result;
    PayloadResult payload_result;
    BmpResult bmp_result;
    StegResult steg_result;
    size_t capacity;
    int status = 1;

    file_result = file_read_all(args->input_file, &secret, &secret_len);
    if (file_result != FILE_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->input_file, file_error_string(file_result));
        goto cleanup;
    }

    payload_result = payload_encode(secret, secret_len, args->input_file, &args->crypto, &blob, &blob_len);
    if (payload_result != PAYLOAD_OK)
    {
        fprintf(stderr, "error: %s\n", payload_error_string(payload_result));
        goto cleanup;
    }

    file_result = file_read_all(args->carrier_file, &bmp_data, &bmp_size);
    if (file_result != FILE_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->carrier_file, file_error_string(file_result));
        goto cleanup;
    }

    bmp_result = bmp_parse(bmp_data, bmp_size, &bmp);
    if (bmp_result != BMP_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->carrier_file, bmp_error_string(bmp_result));
        goto cleanup;
    }

    capacity = steg_capacity(args->steg_algorithm, bmp.body_len);
    if (blob_len > capacity)
    {
        fprintf(stderr,
                "error: payload too large: %zu bytes required, maximum capacity: %zu bytes\n", blob_len, capacity);
        goto cleanup;
    }

    steg_result = steg_embed(args->steg_algorithm, bmp.body, bmp.body_len, blob, blob_len);
    if (steg_result != STEG_OK)
    {
        fprintf(stderr, "error: %s\n", steg_error_string(steg_result));
        goto cleanup;
    }

    file_result = file_write_all(args->output_file, bmp.data, bmp.size);
    if (file_result != FILE_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->output_file, file_error_string(file_result));
        goto cleanup;
    }

    status = 0;

cleanup:
    free(blob);
    free(secret);
    free(bmp_data);
    return status;
}

static int run_extract(const Args *args)
{
    uint8_t *bmp_data = NULL;
    size_t bmp_size = 0;
    uint8_t *blob = NULL;
    Payload payload = {0};
    char *out_path = NULL;
    Bmp bmp;
    FileResult file_result;
    BmpResult bmp_result;
    StegResult steg_result;
    PayloadResult payload_result;
    size_t capacity;
    int status = 1;

    file_result = file_read_all(args->carrier_file, &bmp_data, &bmp_size);
    if (file_result != FILE_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->carrier_file, file_error_string(file_result));
        goto cleanup;
    }

    bmp_result = bmp_parse(bmp_data, bmp_size, &bmp);
    if (bmp_result != BMP_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->carrier_file, bmp_error_string(bmp_result));
        goto cleanup;
    }

    capacity = steg_capacity(args->steg_algorithm, bmp.body_len);
    if (capacity < 4)
    {
        fprintf(stderr, "error: bmp is too small to contain a payload\n");
        goto cleanup;
    }

    blob = malloc(capacity);
    if (blob == NULL)
    {
        fprintf(stderr, "error: out of memory\n");
        goto cleanup;
    }

    steg_result = steg_extract(args->steg_algorithm, bmp.body, bmp.body_len, blob, capacity);
    if (steg_result != STEG_OK)
    {
        fprintf(stderr, "error: %s\n", steg_error_string(steg_result));
        goto cleanup;
    }

    payload_result = payload_decode(blob, capacity, &args->crypto, &payload);
    if (payload_result != PAYLOAD_OK)
    {
        fprintf(stderr, "error: %s\n", payload_error_string(payload_result));
        goto cleanup;
    }

    out_path = path_with_extension(args->output_file, payload.extension);
    if (out_path == NULL)
    {
        fprintf(stderr, "error: out of memory\n");
        goto cleanup;
    }

    file_result = file_write_all(out_path, payload.data, payload.data_len);
    if (file_result != FILE_OK)
    {
        fprintf(stderr, "error: %s: %s\n", out_path, file_error_string(file_result));
        goto cleanup;
    }

    status = 0;

cleanup:
    free(out_path);
    payload_free(&payload);
    free(blob);
    free(bmp_data);
    return status;
}

static const char *const STEG_NAMES[] = {"LSB1", "LSB4", "LSBI"};

static void print_finding(const char *steg_name, const AnalyzeFinding *finding)
{
    switch (finding->status)
    {
    case ANALYZE_NOTHING:
        printf("%s: nothing found\n", steg_name);
        break;
    case ANALYZE_PLAIN:
        printf("%s: hidden file found, %zu bytes, extension %s\n", steg_name, finding->payload.data_len,
               finding->payload.extension);
        break;
    case ANALYZE_NEEDS_PASSWORD:
        printf("%s: looks encrypted (%zu bytes), a password is required (use -pass)\n", steg_name,
               finding->declared_len);
        break;
    case ANALYZE_DECRYPTED:
        printf("%s: encrypted file found with %s %s, %zu bytes, extension %s\n", steg_name,
               crypto_algo_name(finding->crypto.algo), crypto_mode_name(finding->crypto.mode),
               finding->payload.data_len, finding->payload.extension);
        break;
    case ANALYZE_NO_MATCH:
        printf("%s: looks encrypted (%zu bytes), but no algorithm and mode worked with that password\n",
               steg_name, finding->declared_len);
        break;
    }
}

// one file per algorithm, so two algorithms finding something don't overwrite each other
static int save_finding(const char *base, const char *steg_name, const AnalyzeFinding *finding)
{
    size_t path_len = strlen(base) + 1 + strlen(steg_name) + strlen(finding->payload.extension) + 1;
    char *path = malloc(path_len);
    FileResult file_result;

    if (path == NULL)
    {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    snprintf(path, path_len, "%s_%s%s", base, steg_name, finding->payload.extension);

    file_result = file_write_all(path, finding->payload.data, finding->payload.data_len);
    if (file_result != FILE_OK)
    {
        fprintf(stderr, "error: %s: %s\n", path, file_error_string(file_result));
        free(path);
        return 1;
    }

    printf("  saved to %s\n", path);
    free(path);

    return 0;
}

static int run_analyze(const Args *args)
{
    uint8_t *bmp_data = NULL;
    size_t bmp_size = 0;
    Bmp bmp;
    FileResult file_result;
    BmpResult bmp_result;
    int status = 1;

    file_result = file_read_all(args->carrier_file, &bmp_data, &bmp_size);
    if (file_result != FILE_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->carrier_file, file_error_string(file_result));
        goto cleanup;
    }

    bmp_result = bmp_parse(bmp_data, bmp_size, &bmp);
    if (bmp_result != BMP_OK)
    {
        fprintf(stderr, "error: %s: %s\n", args->carrier_file, bmp_error_string(bmp_result));
        goto cleanup;
    }

    for (StegAlgo algo = LSB1; algo < STEG_ALGO_COUNT; algo++)
    {
        AnalyzeFinding finding;
        AnalyzeResult analyze_result = analyze_algorithm(algo, &bmp, &args->crypto, &finding);
        int save_failed = 0;

        if (analyze_result != ANALYZE_OK)
        {
            fprintf(stderr, "error: %s: %s\n", STEG_NAMES[algo], analyze_error_string(analyze_result));
            goto cleanup;
        }

        print_finding(STEG_NAMES[algo], &finding);

        if (args->output_file != NULL && (finding.status == ANALYZE_PLAIN || finding.status == ANALYZE_DECRYPTED))
        {
            save_failed = save_finding(args->output_file, STEG_NAMES[algo], &finding);
        }

        analyze_finding_free(&finding);

        if (save_failed)
        {
            goto cleanup;
        }
    }

    status = 0;

cleanup:
    free(bmp_data);
    return status;
}

static void print_args_error(const Args *args, ArgsResult result)
{
    fprintf(stderr, "error: %s", args_error_string(result));

    if (args->bad_option != NULL)
    {
        fprintf(stderr, " (%s", args->bad_option);

        // never echo a password back to the terminal
        if (args->bad_value != NULL && strcmp(args->bad_option, "-pass") != 0)
        {
            fprintf(stderr, " %s", args->bad_value);
        }

        fprintf(stderr, ")");
    }

    fprintf(stderr, "\n%s", args_usage());
}

int main(int argc, char const *argv[])
{
    Args args;
    ArgsResult args_result = args_parse(argc, argv, &args);

    if (args_result != ARGS_OK)
    {
        print_args_error(&args, args_result);
        return 1;
    }

    if (crypto_options_ignored(&args.crypto))
    {
        fprintf(stderr, "warning: -a/-m ignored without -pass\n");
    }

    if (args.operation == OP_EMBED)
    {
        return run_embed(&args);
    }

    if (args.operation == OP_ANALYZE)
    {
        return run_analyze(&args);
    }

    return run_extract(&args);
}
