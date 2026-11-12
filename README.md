# TP de Implementación: Estenografía - Criptografía y Seguridad (72.04)

<!-- TODO -->

## Integrantes

- Arcodaci, Tiziano - 64637
- Carranza, Máximo Daniel - 64538
- Lee, Sebastián Nicolás - 64675
- Villanueva, Julia - 69619

## Devs

### Dependencias

```bash
sudo apt install build-essential libssl-dev clang-format
pip install pre-commit
```
> pampero.itba.edu.ar: Arch Linux rolling x86_64; GCC 16.2.1, GNU Make 4.4.1, OpenSSL 3.6.3

### pre-commit

```bash
pre-commit install
```

> Config file: `.pre-commit-config.yaml`

## Build and execute

Compile code with `make`, generates `bin/stegobmp` for execution:

```bash
make all
make debug
make clean
```

```bash
./bin/stegobmp 
```

<!-- TODO flags y demás cosas para el binario stegobmp -->
