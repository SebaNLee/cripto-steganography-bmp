# Implementation Assignment: Steganography - Cryptography and Security (72.04)

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

```bash
./bin/stegobmp -analyze -p <bitmap.bmp> [-out <base>] [-a <aes128|aes192|aes256|3des>] [-m <ecb|cfb|ofb|cbc>] [-pass <password>]
```

Tries `LSB1`, `LSB4` and `LSBI` and prints a report to STDOUT with what each one found.

- If the hidden data looks encrypted and there is no `-pass`, the report says a password is required.
- With `-pass` and no `-a` or `-m`, every algorithm and mode is tried. Giving `-a` or `-m` limits the search to it.
- With `-out`, each finding is saved as `<base>_<algorithm><extension>`, for example `found_LSB4.png`. Without it, only the report is printed.
- `-steg` and `-in` are not accepted.

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
```

### Analyze examples

```bash
./bin/stegobmp -analyze -p files/ejemplo/ladoLSB4.bmp
./bin/stegobmp -analyze -p files/ejemplo/ladoLSBIdescfb.bmp
./bin/stegobmp -analyze -p files/ejemplo/ladoLSBIdescfb.bmp -pass margarita -out ./found
```
