#ifndef STEG_H
#define STEG_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Available steganography algorithms, strategy pattern.
 */
typedef enum
{
    stegAlgoLsb1 = 0,
    stegAlgoLsb4,
    stegAlgoLsbi,
    stegAlgoCount
} StegAlgo;

/**
 * @brief Result values of a steganography function.
 */
typedef enum
{
    stegOk = 0,
    stegErrInvalidArg,
    stegErrUnsupported,
    stegErrCapacity
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

#endif
