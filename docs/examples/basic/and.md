# AND

Ejemplo de compuerta AND usando dos entradas con pull-up y un LED activo en bajo.

## Qué Aprendes

- Invertir entradas activas en bajo.
- Construir lógica combinacional.
- Mapear el resultado a un LED activo en bajo.

## Archivos

- `and/devlab.toml`
- `and/devlab-vhdl.toml`
- `and/src/top.v`
- `and/src/top.vhd`
- `and/pins.cst`

## Compilar

```bash
cd and
devlab build
devlab flash
```

## VHDL

```bash
cd and
devlab build -c devlab-vhdl.toml
devlab flash
```

## Lógica

Las entradas físicas se invierten dentro del HDL para que el valor lógico sea `1` cuando el botón está presionado. El LED se enciende cuando ambas entradas lógicas valen `1`.
