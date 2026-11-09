#include "lib/bit.h"

unsigned stream_bit_get(const uint8_t *buf, size_t bit_index)
{
    size_t byte_index = bit_index >> 0x03;
    size_t bit_offset = bit_index & 0x07;
    size_t shift = 0x07 - bit_offset;
    uint8_t value = buf[byte_index];
    unsigned bit = (value >> shift) & 0x01;

    return bit;
}

void stream_bit_set(uint8_t *buf, size_t bit_index, unsigned bit)
{
    size_t byte_index = bit_index >> 0x03;
    size_t bit_offset = bit_index & 0x07;
    size_t shift = 0x07 - bit_offset;
    uint8_t mask = (uint8_t)(0x01 << shift);

    if ((bit & 0x01) != 0)
    {
        buf[byte_index] |= mask;
    }
    else
    {
        buf[byte_index] &= (uint8_t)~mask;
    }
}

uint8_t stream_nibble_get(const uint8_t *buf, size_t nibble_index)
{
    size_t byte_index = nibble_index >> 0x01;
    uint8_t value = buf[byte_index];
    uint8_t nibble;

    if ((nibble_index & 0x01) == 0)
    {
        nibble = (uint8_t)((value >> 0x04) & 0x0F);
    }
    else
    {
        nibble = (uint8_t)(value & 0x0F);
    }

    return nibble;
}

void stream_nibble_set(uint8_t *buf, size_t nibble_index, uint8_t nibble)
{
    size_t byte_index = nibble_index >> 0x01;
    uint8_t value = buf[byte_index];
    uint8_t new_nibble = (uint8_t)(nibble & 0x0F);

    if ((nibble_index & 0x01) == 0)
    {
        value = (uint8_t)((value & 0x0F) | (new_nibble << 0x04));
    }
    else
    {
        value = (uint8_t)((value & 0xF0) | new_nibble);
    }

    buf[byte_index] = value;
}

// TODO maybe move to payload.c
uint32_t be32_read(const uint8_t *buf)
{
    uint32_t value = 0;

    value |= (uint32_t)buf[0] << 0x18;
    value |= (uint32_t)buf[1] << 0x10;
    value |= (uint32_t)buf[2] << 0x08;
    value |= (uint32_t)buf[3];

    return value;
}

// TODO maybe move to payload.c
void be32_write(uint8_t *buf, uint32_t value)
{
    buf[0] = (uint8_t)(value >> 0x18);
    buf[1] = (uint8_t)(value >> 0x10);
    buf[2] = (uint8_t)(value >> 0x08);
    buf[3] = (uint8_t)value;
}
