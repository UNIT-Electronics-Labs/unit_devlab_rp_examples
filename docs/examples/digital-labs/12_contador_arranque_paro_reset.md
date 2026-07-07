# 12 Contador con Arranque, Paro y Reset

Práctica de contador con control de ejecución.

## Objetivo

Agregar control de arranque, paro y reset a un circuito secuencial.

## Control

- `key_enable_n`: funciona como arranque/paro.
- `key_reset_n`: reset activo en bajo.

## Verilog

```bash
cd digital-labs/12_contador_arranque_paro_reset
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/12_contador_arranque_paro_reset
devlab build -c devlab-vhdl.toml
devlab flash
```

## Conceptos

- Enable de contador.
- Reset.
- Conservación de estado cuando el contador está detenido.
