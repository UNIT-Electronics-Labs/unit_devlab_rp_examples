# 02 Restador N bits

Práctica de resta binaria en paralelo. Instancia `nbit_subtractor`.

## Objetivo

Entender la resta como operación combinacional parametrizable y observar un bit de diferencia en el LED.

## Salida

- `led_n`: `diff[0]`, activo en bajo.

## Verilog

```bash
cd digital-labs/02_restador_n_bits
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/02_restador_n_bits
devlab build -c devlab-vhdl.toml
devlab flash
```

## Archivos Clave

- `src/digital_labs.v`: módulo `nbit_subtractor`.
- `src/top.v`: conexión Verilog a entradas y LED.
- `src/top.vhd`: versión VHDL autocontenida.
