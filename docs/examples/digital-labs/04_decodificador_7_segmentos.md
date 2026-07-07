# 04 Decodificador de 7 Segmentos

Práctica de decodificación para exhibidor de 7 segmentos. Instancia `seven_segment_decoder`.

## Objetivo

Convertir un valor binario en las señales de segmentos `a` a `g`.

## Demostración en Placa

El `top` muestra en `led_n` el segmento `a` del número formado con dos entradas.

## Verilog

```bash
cd digital-labs/04_decodificador_7_segmentos
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/04_decodificador_7_segmentos
devlab build -c devlab-vhdl.toml
devlab flash
```

## Archivos Clave

- `src/digital_labs.v`: módulo `seven_segment_decoder`.
- `src/top.v`: conexión Verilog.
- `src/top.vhd`: versión VHDL.
