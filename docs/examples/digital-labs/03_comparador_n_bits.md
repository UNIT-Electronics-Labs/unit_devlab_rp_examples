# 03 Comparador N bits

Práctica de comparación binaria. Instancia `nbit_comparator`.

## Objetivo

Comparar dos vectores y generar banderas como igualdad, mayor que o menor que.

## Salida

- `led_n`: encendido cuando `a == b`.

## Verilog

```bash
cd digital-labs/03_comparador_n_bits
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/03_comparador_n_bits
devlab build -c devlab-vhdl.toml
devlab flash
```

## Archivos Clave

- `src/digital_labs.v`: módulo `nbit_comparator`.
- `src/top.v`: top-level Verilog.
- `src/top.vhd`: versión VHDL.
