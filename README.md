# Steganography - Criptografía y Seguridad (72.04)

## Members

- Arcodaci, Tiziano - 64637
- Carranza, Máximo Daniel - 64538
- Lee, Sebastián Nicolás - 64675
- Villanueva, Julia - 69619

## Development

### Dependencies

- C99 compiler
- GNU Make
- OpenSSL

```bash
sudo apt install build-essential libssl-dev clang-format
pip install pre-commit
```
> pampero.itba.edu.ar: Arch Linux rolling x86_64; GCC 16.2.1, GNU Make 4.4.1, OpenSSL 3.6.3

### pre-commit

```bash
pip install pre-commit
pre-commit install
```

> Config file: `.pre-commit-config.yaml`

## Build and execute

Compile code with `make`, generates `bin/stegobmp` for execution:

```bash
make all
```

Other useful commands:

```bash
make debug
make clean
```

## Usage

### Embed

```bash
./bin/stegobmp -embed -in <file> -p <bitmap.bmp> -out <bitmap.bmp> -steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] [-pass <password>]
```

### Extract

```bash
./bin/stegobmp -extract -p <bitmap.bmp> -out <file> -steg <LSB1|LSB4|LSBI> [-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] [-pass <password>]
```

> Note: -extract adds the file extension automatically if there is none in `-out`.

### Analyze

Tries all the LSB algorithms, cripgoraphy algorithms (if pass `-pass` is included), and other very specific analysis to optimize our group ZIP analysis. Prints a report to STDOUT with what have been found including: hidden message, possible encrypted data, and trailing data.

```bash
./bin/stegobmp -analyze -p <bitmap.bmp> [-out <base>] [-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] [-pass <password>]
```

## Flags

| Flag | Description |
|---|---|
| `-embed` | Embed a file in a BMP |
| `-extract` | Extract a hidden file from a BMP |
| `-analyze` | Look for a hidden file with every steganography algorithm |
| `-in <file>` | Input file to hide |
| `-p <bmp>` | Carrier BMP |
| `-out <path>` | Output file path (a base name with `-analyze`, where it is optional) |
| `-steg ...` | Steganography algorithm: `LSB1`, `LSB4` or `LSBI` |
| `-a ...` | Encryption algorithm: `aes128`, `aes192`, `aes256` or `3des` |
| `-m ...` | Encryption mode: `ecb`, `cfb`, `ofb` or `cbc` |
| `-pass password` | Encryption password |

## Steganalysis

### General examples

```bash
./bin/stegobmp -extract -p files/ejemplo/ladoLSB1.bmp -out ./ -steg LSB1
./bin/stegobmp -extract -p files/ejemplo/ladoLSB4.bmp -out ./ -steg LSB4
./bin/stegobmp -extract -p files/ejemplo/ladoLSBI.bmp -out ./ -steg LSBI
./bin/stegobmp -extract -p files/ejemplo/ladoLSB1aes128cbc.bmp -out ./ -steg LSB1 -pass margarita
./bin/stegobmp -extract -p files/ejemplo/ladoLSBIaes256ofb.bmp -out ./ -steg LSBI -a aes256 -m ofb -pass margarita
./bin/stegobmp -extract -p files/ejemplo/ladoLSBIdescfb.bmp -out ./ -steg LSBI -a 3des -m cfb -pass margarita
./bin/stegobmp -analyze -p files/ejemplo/ladoLSBIdescfb.bmp
./bin/stegobmp -analyze -p files/ejemplo/ladoLSBIdescfb.bmp -pass margarita -out ./found
```

### Group examples

```bash
mkdir results

# bogota.bmp: LSBI; fuerza bruta (fue probar todos)
# Se obtiene la password que es "conocido" con "\r" final
./bin/stegobmp -analyze -p files/grupo6/bogota.bmp -out ./results/bogota

# django.bmp: LSB4; fuerza bruta (fue probar todos)
./bin/stegobmp -analyze -p files/grupo6/django.bmp -out ./results/django

# frozen.bmp: con analyze
# looks encrypted, necesita -pass
./bin/stegobmp -analyze -p files/grupo6/frozen.bmp -out ./results/frozen

# medianoche1.bmp: 
# se obtiene: "al .png cambiar extension por .zip y descomprimir"
./bin/stegobmp -analyze -p files/grupo6/medianoche1.bmp -out ./results/medianoche1

# se extrae como zip y se obtiene sol6.txt con pistas del buscaminas, se saca: "aescbc"
cp ./results/django_LSB4.png ./results/django_LSB4.zip
unzip ./results/django_LSB4.zip -d ./results/

# frozen.bmp: LSB1 AES192 CBC pass:'conocido\r'
# video: "O nuestro agente ocultó datos en el archivo" (con fuerte acento español)
./bin/stegobmp -extract -p files/grupo6/frozen.bmp -out ./results/frozen -steg LSB1 -a aes192 -m cbc -pass $'conocido\r'

# o también, con analyze:
./bin/stegobmp -analyze -p files/grupo6/frozen.bmp -out ./results/frozen -pass $'conocido\r'
```

> Nota: Este es un flujo de análisis elegante al que se puede llegar con los comandos actuales. En realidad, se analizó y probó muchas más combinaciones y ramas no redactadas acá para llegar a este flujo final resumido. `-analyze` es la conclusión de todo esto y sirve como herramienta de alto nivel para el análisis de el zip del grupo (y tal vez otros más).

Otros comandos intermedios que nos sirvieron:

```bash
# se ve que al final hay un "sol6.txtUT" raro, se asume que debe ser un archivo oculto
strings -a ./results/django.png > ./results/django.txt

```