# 05 Decodificador de 16 Segmentos

Práctica de decodificación para exhibidor de 16 segmentos. Instancia `sixteen_segment_decoder`.

## Objetivo

Extender la idea del decodificador de 7 segmentos a más segmentos para representar símbolos con mayor detalle.

## Demostración en Placa

El `top` actual muestra en `led_n` uno de los segmentos decodificados.

## Verilog

```bash
cd digital-labs/05_decodificador_16_segmentos
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/05_decodificador_16_segmentos
devlab build -c devlab-vhdl.toml
devlab flash
```

## Archivos Clave

- `src/digital_labs.v`: módulo `sixteen_segment_decoder`.
- `src/top.v`: conexión Verilog.
- `src/top.vhd`: versión VHDL.
