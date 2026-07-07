# 11 Contador Ascendente-Descendente

Práctica de contador con dirección configurable.

## Objetivo

Usar una entrada de control para elegir entre incrementar o decrementar.

## Control

- `key_enable_n`: selecciona la dirección.

## Verilog

```bash
cd digital-labs/11_contador_ascendente_descendente
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/11_contador_ascendente_descendente
devlab build -c devlab-vhdl.toml
devlab flash
```

## Conceptos

- Multiplexado de siguiente estado.
- Entrada de dirección.
- Estado registrado.
