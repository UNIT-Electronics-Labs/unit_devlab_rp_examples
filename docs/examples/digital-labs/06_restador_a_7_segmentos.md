# 06 Restador a 7 Segmentos

Práctica que combina una resta binaria con decodificación a 7 segmentos. Instancia `subtract_to_7seg`.

## Objetivo

Encadenar bloques combinacionales: primero calcular una resta y después traducir el resultado a segmentos.

## Verilog

```bash
cd digital-labs/06_restador_a_7_segmentos
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/06_restador_a_7_segmentos
devlab build -c devlab-vhdl.toml
devlab flash
```

## Archivos Clave

- `src/digital_labs.v`: módulos de resta y decodificación.
- `src/top.v`: top-level Verilog.
- `src/top.vhd`: versión VHDL.
