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
    size_t (*capacity)(size_t bodyLen);
    StegResult (*embed)(uint8_t *body, size_t bodyLen, uint8_t *data, size_t dataLen);
    StegResult (*extract)(uint8_t *body, size_t bodyLen, uint8_t *out, size_t outLen);
} StegOps;

/**
 *  Each steganography algorithm file should export their own StegOps implementation.
 */
extern StegOps lsb1Ops;
extern StegOps lsb4Ops;
extern StegOps lsbiOps;

/**
 * @brief Get the operations for an algorithm.
 *
 * @param algo Algorithm to look up.
 * @return Algorithm operations, or NULL if the algorithm is invalid.
 */
StegOps *stegGetOps(StegAlgo algo);

/**
 * @brief Get the maximum number of hidden bytes for a carrier body.
 *
 * @param algo Algorithm to use.
 * @param bodyLen Carrier body length in bytes.
 * @return Maximum hidden stream length, or 0 if the algorithm is invalid.
 */
size_t stegCapacity(StegAlgo algo, size_t bodyLen);

/**
 * @brief Hide a stream in a BMP body.
 *
 * @param algo Algorithm to use.
 * @param body Carrier body.
 * @param bodyLen Carrier body length in bytes.
 * @param data Stream to hide.
 * @param dataLen Stream length in bytes.
 * @return Operation result.
 */
StegResult stegEmbed(StegAlgo algo, uint8_t *body, size_t bodyLen, uint8_t *data, size_t dataLen);

/**
 * @brief Extract a stream from a BMP body.
 *
 * @param algo Algorithm to use.
 * @param body Carrier body.
 * @param bodyLen Carrier body length in bytes.
 * @param out Destination buffer.
 * @param outLen Number of bytes to extract.
 * @return Operation result.
 */
StegResult stegExtract(StegAlgo algo, uint8_t *body, size_t bodyLen, uint8_t *out, size_t outLen);

/**
 * @brief Get a readable description of an error.
 *
 * @param error Error to describe.
 * @return Error description.
 */
char *stegErrorString(StegResult error);

#endif
