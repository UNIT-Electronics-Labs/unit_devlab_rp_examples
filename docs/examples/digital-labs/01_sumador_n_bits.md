# 01 Sumador N bits

Práctica de suma binaria en paralelo. Instancia `nbit_adder` y muestra `sum[0]` en el LED activo en bajo.

## Objetivo

Comprender como se construye un sumador parametrizable con acarreo de entrada y salida.

## Entradas y Salidas

- `key_reset_n`: bit `a[0]`.
- `key_enable_n`: bit `b[0]`.
- `led_n`: resultado `sum[0]`, activo en bajo.

## Verilog

```bash
cd digital-labs/01_sumador_n_bits
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/01_sumador_n_bits
devlab build -c devlab-vhdl.toml
devlab flash
```

## Archivos Clave

- `src/digital_labs.v`: módulo `nbit_adder`.
- `src/top.v`: conexión Verilog a la placa.
- `src/top.vhd`: versión VHDL autocontenida.
