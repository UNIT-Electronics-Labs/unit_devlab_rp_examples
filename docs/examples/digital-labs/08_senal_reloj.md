# 08 Señal de Reloj

Práctica secuencial para dividir el reloj principal y observarlo en el LED.

## Objetivo

Usar un contador como divisor de frecuencia para transformar un reloj rápido en una señal visible.

## Verilog

```bash
cd digital-labs/08_senal_reloj
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/08_senal_reloj
devlab build -c devlab-vhdl.toml
devlab flash
```

## Conceptos

- `clk` como reloj principal.
- Registro que cambia en flanco de subida.
- Selección de un bit del contador como salida lenta.
