#ifndef STEG_H
#define STEG_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Available steganography algorithms, strategy pattern.
 */
typedef enum
{
    LSB1 = 0,
    LSB4,
    LSBI,
    STEG_ALGO_COUNT
} StegAlgo;

/**
 * @brief Result values of a steganography function.
 */
typedef enum
{
    STEG_OK = 0,
    STEG_ERR_INVALID_ARG,
    STEG_ERR_UNSUPPORTED,
    STEG_ERR_CAPACITY
} StegResult;

/**
 * @brief Operations available for each steganography algorithm.
 */
typedef struct
{
    size_t (*capacity)(size_t body_len);
    StegResult (*embed)(uint8_t *body, size_t body_len, uint8_t *data, size_t data_len);
    StegResult (*extract)(uint8_t *body, size_t body_len, uint8_t *out, size_t out_len);
} StegOps;

/**
 *  Each steganography algorithm file should export their own StegOps implementation.
 */
extern StegOps lsb1_ops;
extern StegOps lsb4_ops;
extern StegOps lsbi_ops;

/**
 * @brief Get the operations for an algorithm.
 *
 * @param algo Algorithm to look up.
 * @return Algorithm operations, or NULL if the algorithm is invalid.
 */
StegOps *steg_get_ops(StegAlgo algo);

/**
 * @brief Get the maximum number of hidden bytes for a carrier body.
 *
 * @param algo Algorithm to use.
 * @param body_len Carrier body length in bytes.
 * @return Maximum hidden stream length, or 0 if the algorithm is invalid.
 */
size_t steg_capacity(StegAlgo algo, size_t body_len);

/**
 * @brief Hide a stream in a BMP body.
 *
 * @param algo Algorithm to use.
 * @param body Carrier body.
 * @param body_len Carrier body length in bytes.
 * @param data Stream to hide.
 * @param data_len Stream length in bytes.
 * @return Operation result.
 */
StegResult steg_embed(StegAlgo algo, uint8_t *body, size_t body_len, uint8_t *data, size_t data_len);

/**
 * @brief Extract a stream from a BMP body.
 *
 * @param algo Algorithm to use.
 * @param body Carrier body.
 * @param body_len Carrier body length in bytes.
 * @param out Destination buffer.
 * @param out_len Number of bytes to extract.
 * @return Operation result.
 */
StegResult steg_extract(StegAlgo algo, uint8_t *body, size_t body_len, uint8_t *out, size_t out_len);

/**
 * @brief Get a readable description of an error.
 *
 * @param error Error to describe.
 * @return Error description.
 */
char *steg_error_string(StegResult error);

#endif
