#include "lib/bit.h"

unsigned streamBitGet(const uint8_t *buf, size_t bitIndex)
{
    size_t byteIndex = bitIndex >> 0x03;
    size_t bitOffset = bitIndex & 0x07;
    size_t shift = 0x07 - bitOffset;
    uint8_t value = buf[byteIndex];
    unsigned bit = (value >> shift) & 0x01;

    return bit;
}

void streamBitSet(uint8_t *buf, size_t bitIndex, unsigned bit)
{
    size_t byteIndex = bitIndex >> 0x03;
    size_t bitOffset = bitIndex & 0x07;
    size_t shift = 0x07 - bitOffset;
    uint8_t mask = (uint8_t)(0x01 << shift);

    if ((bit & 0x01) != 0)
    {
        buf[byteIndex] |= mask;
    }
    else
    {
        buf[byteIndex] &= (uint8_t)~mask;
    }
}

uint8_t streamNibbleGet(const uint8_t *buf, size_t nibbleIndex)
{
    size_t byteIndex = nibbleIndex >> 0x01;
    uint8_t value = buf[byteIndex];
    uint8_t nibble;

    if ((nibbleIndex & 0x01) == 0)
    {
        nibble = (uint8_t)((value >> 0x04) & 0x0F);
    }
    else
    {
        nibble = (uint8_t)(value & 0x0F);
    }

    return nibble;
}

void streamNibbleSet(uint8_t *buf, size_t nibbleIndex, uint8_t nibble)
{
    size_t byteIndex = nibbleIndex >> 0x01;
    uint8_t value = buf[byteIndex];
    uint8_t newNibble = (uint8_t)(nibble & 0x0F);

    if ((nibbleIndex & 0x01) == 0)
    {
        value = (uint8_t)((value & 0x0F) | (newNibble << 0x04));
    }
    else
    {
        value = (uint8_t)((value & 0xF0) | newNibble);
    }

    buf[byteIndex] = value;
}

// TODO maybe move to payload.c
uint32_t be32Read(const uint8_t *buf)
{
    uint32_t value = 0;

    value |= (uint32_t)buf[0] << 0x18;
    value |= (uint32_t)buf[1] << 0x10;
    value |= (uint32_t)buf[2] << 0x08;
    value |= (uint32_t)buf[3];

    return value;
}

// TODO maybe move to payload.c
void be32Write(uint8_t *buf, uint32_t value)
{
    buf[0] = (uint8_t)(value >> 0x18);
    buf[1] = (uint8_t)(value >> 0x10);
    buf[2] = (uint8_t)(value >> 0x08);
    buf[3] = (uint8_t)value;
}
