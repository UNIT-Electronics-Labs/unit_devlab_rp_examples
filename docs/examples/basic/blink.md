# Blink

Ejemplo minimo para comprobar reloj, sintesis, place-and-route y carga en una FPGA Gowin.

## Qué Aprendes

- Usar `clk` como reloj de sistema.
- Implementar un contador.
- Observar un bit del contador en el LED.
- Compilar y cargar con DevLab.

## Archivos

- `blink/devlab.toml`: build Verilog.
- `blink/devlab-vhdl.toml`: build VHDL.
- `blink/src/top.v`: implementacion Verilog.
- `blink/src/top.vhd`: implementacion VHDL.
- `blink/pins.cst`: pines de la placa.

## Compilar

```bash
cd blink
devlab build
devlab flash
```

## VHDL

```bash
cd blink
devlab build -c devlab-vhdl.toml
devlab flash
```

## Lectura del Circuito

El contador incrementa en cada flanco de subida de `clk`. Un bit alto del contador cambia lento y se conecta al LED para producir parpadeo visible.
