# OR

Ejemplo de compuerta OR con dos entradas y un LED.

## Qué Aprendes

- Describir una operación OR en HDL.
- Separar señales físicas activas en bajo de señales lógicas internas.
- Usar el mismo ejemplo en Verilog y VHDL.

## Compilar

```bash
cd or
devlab build
devlab flash
```

## VHDL

```bash
cd or
devlab build -c devlab-vhdl.toml
devlab flash
```

## Lógica

El LED se activa cuando al menos una de las dos entradas lógicas vale `1`.
