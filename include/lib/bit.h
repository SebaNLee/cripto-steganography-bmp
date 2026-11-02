#ifndef BIT_H
#define BIT_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Read one bit from a byte.
 *
 * @param value Byte to inspect.
 * @param position Bit position in [0, 7], where 0 is the LSB.
 * @return The inspected bit.
 *
 * @pre position < 8.
 */
static inline uint8_t bitGet(uint8_t value, uint8_t position)
{
    return (uint8_t)((value >> position) & 0x01);
}

/**
 * @brief Write one bit into a byte.
 *
 * @param value Byte to write.
 * @param position Bit position in [0, 7], where 0 is the LSB.
 * @param bit Bit to write, where the LSB is used.
 * @return Byte with the selected bit replaced.
 *
 * @pre position < 8.
 */
static inline uint8_t bitSet(uint8_t value, uint8_t position, uint8_t bit)
{
    uint8_t mask = (uint8_t)(0x01 << position);

    return (bit & 0x01) ? (uint8_t)(value | mask) : (uint8_t)(value & (uint8_t)~mask);
}

/**
 * @brief Read the least significant bit of a byte.
 *
 * @param value Byte to inspect.
 * @return The inspected LSB.
 */
static inline uint8_t lsbGet(uint8_t value)
{
    return bitGet(value, 0);
}

/**
 * @brief Write the least significant bit of a byte.
 *
 * @param value Byte to write.
 * @param bit Bit to write, where only its least significant bit is used.
 * @return Byte with its LSB replaced.
 */
static inline uint8_t lsbSet(uint8_t value, uint8_t bit)
{
    return bitSet(value, 0, bit);
}

/**
 * @brief Read the low nibble of a byte.
 *
 * @param value Byte to inspect.
 * @return Value of bits 0 through 3, using ranges [0, 15].
 */
static inline uint8_t lowNibbleGet(uint8_t value)
{
    return (uint8_t)(value & 0x0F);
}

/**
 * @brief Write the low nibble of a byte.
 *
 * @param value Byte to write.
 * @param nibble Low nibble, where only its four least significant bits are used.
 * @return Byte with its low nibble replaced.
 */
static inline uint8_t lowNibbleSet(uint8_t value, uint8_t nibble)
{
    return (uint8_t)((value & 0xF0) | (nibble & 0x0F));
}

#endif
