#include "lib/bit.h"
#include "steg/steg.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    // TODO: orquestacion de -embed, -extract, cli, enc, etc

    // embed:   read_file -> payload_encode -> bmp_load -> steg_capacity (chequeo) -> steg_embed -> bmp_save
    // extract: bmp_load -> steg_extract(4) -> payload_declared_size -> steg_extract(4+n) -> payload_decode -> write_file(out + ext)

    /* TODO: cableado
     *
     * payload = formato del mensaje oculto  (BE32 || datos || .ext, + cifrado), usa lib de cifrado si se pide
     * steg    = mapeo de esos bytes al cuerpo del bmp (bits, canales, patrones)
     *
     * embed:   read_file(-in)                 -> secret
     *          payload_encode(secret, enc)    -> blob   (formato, no toca el bmp)
     *          bmp_load(-p)                   -> bmp
     *          steg_capacity >= blob_len         (chequeo antes de escribir)
     *          steg_embed(algo, bmp, blob)       (bits sobre el cuerpo)
     *          bmp_save(-out)
     *
     * extract: bmp_load(-p)                   -> bmp
     *          steg_extract(4)                -> head
     *          payload_blob_len(head)         -> total = 4 + n
     *          steg_extract(total)            -> all
     *          payload_decode(all, enc)       -> secret + ext
     *          write_file(out + ext)
     *
     * reglas: solo main imprime; main libera; capacidad antes de escribir.
     */

    // TODO delete bit lib test
    (void)argc;
    (void)argv;
    uint8_t temp[4];
    be32_write(temp, 0x00CACA);
    printf("be32: %08X (should be 0000CACA)\n", (unsigned)be32_read(temp));
    printf("lsb: %u (should be 1)\n", lsb_get(lsb_set(0xAA, 1)));
    printf("nibble: %02X (should be A5)\n", (unsigned)low_nibble_set(0xAB, 0x05));
    printf("bit3: %u (should be 1)\n", bit_get(bit_set(0x00, 3, 1), 3));
    // TODO delete bit lib test

    // TODO test steg
    uint8_t body[256];
    uint8_t data[8] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
    uint8_t out[8];

    memset(body, 0, sizeof(body));
    memset(out, 0, sizeof(out));
    steg_embed(LSB1, body, sizeof(body), data, sizeof(data));
    steg_extract(LSB1, body, sizeof(body), out, sizeof(out));
    printf("LSB1: %d (should be 1)\n", memcmp(data, out, sizeof(data)) == 0);

    memset(body, 0, sizeof(body));
    memset(out, 0, sizeof(out));
    steg_embed(LSB4, body, sizeof(body), data, sizeof(data));
    steg_extract(LSB4, body, sizeof(body), out, sizeof(out));
    printf("LSB4: %d (should be 1)\n", memcmp(data, out, sizeof(data)) == 0);

    memset(body, 0, sizeof(body));
    memset(out, 0, sizeof(out));
    steg_embed(LSBI, body, sizeof(body), data, sizeof(data));
    steg_extract(LSBI, body, sizeof(body), out, sizeof(out));
    printf("LSBI: %d (should be 1)\n", memcmp(data, out, sizeof(data)) == 0);
    // TODO test steg

    return 0;
}
