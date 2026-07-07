# Introducción a Verilog

Verilog describe hardware. No es un lenguaje de software secuencial: un `assign`, una compuerta y varios bloques `always` representan logica que existe en paralelo dentro de la FPGA.

## Modulo

Un modulo define entradas, salidas e implementacion interna.

```verilog
module top (
    input  wire key_reset_n,
    input  wire key_enable_n,
    output wire led_n
);
    wire a = ~key_reset_n;
    wire b = ~key_enable_n;

    assign led_n = ~(a & b);
endmodule
```

## Señales

- `wire`: conexión combinacional.
- `reg` o `logic`: señal asignada dentro de un bloque procedural.
- `input`: entrada física o señal superior.
- `output`: salida física o señal superior.

## Lógica Combinacional

La lógica combinacional no necesita reloj. El resultado cambia cuando cambian sus entradas.

```verilog
assign y = (a & b) | c;
```

## Lógica Secuencial

La lógica secuencial usa reloj. Sirve para contadores, registros y máquinas de estado.

```verilog
always @(posedge clk) begin
    if (!key_reset_n)
        counter <= 0;
    else
        counter <= counter + 1;
end
```

## Relación con los ejemplos

En cada práctica:

- `src/top.v` conecta la práctica con pines reales.
- `src/digital_labs.v` contiene los módulos de la práctica.
- `pins.cst` asigna nombres de señales a pines físicos del target.

Para compilar Verilog:

```bash
devlab build
```
