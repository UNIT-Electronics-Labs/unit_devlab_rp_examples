# 07 ALU N bits

Práctica de unidad aritmética y lógica. Instancia `nbit_alu`.

## Objetivo

Combinar varias operaciones aritméticas y lógicas en un solo bloque controlado por `op`.

## Demostración en Placa

El `top` usa operación suma (`op = 0`).

## Verilog

```bash
cd digital-labs/07_alu_n_bits
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/07_alu_n_bits
devlab build -c devlab-vhdl.toml
devlab flash
```

## Archivos Clave

- `src/digital_labs.v`: módulo `nbit_alu`.
- `src/top.v`: selección de operación y conexión a pines.
- `src/top.vhd`: versión VHDL.
