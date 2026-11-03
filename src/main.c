#include <stdio.h>

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

    return 0;
}
