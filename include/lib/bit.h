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
static inline uint8_t bit_get(uint8_t value, uint8_t position)
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
static inline uint8_t bit_set(uint8_t value, uint8_t position, uint8_t bit)
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
static inline uint8_t lsb_get(uint8_t value)
{
    return bit_get(value, 0);
}

/**
 * @brief Write the least significant bit of a byte.
 *
 * @param value Byte to write.
 * @param bit Bit to write, where only its least significant bit is used.
 * @return Byte with its LSB replaced.
 */
static inline uint8_t lsb_set(uint8_t value, uint8_t bit)
{
    return bit_set(value, 0, bit);
}

/**
 * @brief Read the low nibble of a byte.
 *
 * @param value Byte to inspect.
 * @return Value of bits 0 through 3, using ranges [0, 15].
 */
static inline uint8_t low_nibble_get(uint8_t value)
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
static inline uint8_t low_nibble_set(uint8_t value, uint8_t nibble)
{
    return (uint8_t)((value & 0xF0) | (nibble & 0x0F));
}

/**
 * @brief Read a bit from a byte buffer in MSB order.
 *
 * @param buf Buffer to inspect.
 * @param bit_index Bit index.
 * @return The inspected bit.
 *
 * @pre buf has at least (bit_index / 8) + 1 bytes.
 */
unsigned stream_bit_get(const uint8_t *buf, size_t bit_index);

/**
 * @brief Write a bit into a byte buffer in MSB order.
 *
 * @param buf Buffer to write.
 * @param bit_index Bit index.
 * @param bit Bit to write, where only its least significant bit is used.
 *
 * @pre buf has at least (bit_index / 8) + 1 bytes.
 */
void stream_bit_set(uint8_t *buf, size_t bit_index, unsigned bit);

/**
 * @brief Read a nibble from a byte buffer in high nibble first order.
 *
 * @param buf Buffer to inspect.
 * @param nibble_index Nibble index.
 * @return The inspected nibble.
 *
 * @pre buf has at least (nibble_index / 2) + 1 bytes.
 */
uint8_t stream_nibble_get(const uint8_t *buf, size_t nibble_index);

/**
 * @brief Write a nibble into a byte buffer in high nibble first order.
 *
 * @param buf Buffer to write.
 * @param nibble_index Nibble index.
 * @param nibble Nibble to write, where only its four least significant bits are used.
 *
 * @pre buf has at least (nibble_index / 2) + 1 bytes.
 */
void stream_nibble_set(uint8_t *buf, size_t nibble_index, uint8_t nibble);

/**
 * @brief Read a 32bit unsigned integer in big-endian order.
 *
 * @param buf Buffer to inspect.
 * @return The inspected value.
 *
 * @pre buf points to at least 4 readable bytes.
 */
uint32_t be32_read(const uint8_t *buf);

/**
 * @brief Write a 32bit unsigned integer in big-endian order.
 *
 * @param buf Buffer to write.
 * @param value Value to write.
 *
 * @pre buf points to at least 4 writable bytes.
 */
void be32_write(uint8_t *buf, uint32_t value);

#endif
