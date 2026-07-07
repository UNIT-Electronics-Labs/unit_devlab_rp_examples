# Archivos CST

Un archivo `.cst` define la distribución física de entradas y salidas de la FPGA Gowin. En otras palabras: conecta los nombres que usas en Verilog o VHDL con pines reales del target.

DevLab usa este archivo desde `devlab.toml`:

```toml
[build]
top = "top"
sources = ["src/top.v"]
constraints = "pins.cst"
build_dir = "build"
```

## Relación Entre HDL, Chip y Target

Hay tres niveles distintos:

- HDL: define puertos logicos como `clk`, `led_n` o `key_reset_n`.
- Chip Gowin: define familia, dispositivo y bancos de I/O, por ejemplo `GW1N-9C` y `GW1NR-LV9QN88PC6/I5`.
- Target: define la tarjeta concreta, sus conectores, botones, LEDs, reloj y programador.

El `.cst` vive entre el HDL y el target: traduce puertos logicos a pines fisicos. Por eso no conviene escribir la documentacion como si el SDK perteneciera a una sola tarjeta. La tarjeta de referencia puede cambiar; el concepto de distribucion de pines se mantiene.

## Relación Entre HDL y CST

Si tu `top.v` declara estos puertos:

```verilog
module top (
    input  wire clk,
    input  wire key_reset_n,
    input  wire key_enable_n,
    output wire led_n
);
endmodule
```

Entonces el `pins.cst` debe usar exactamente esos mismos nombres:

```text
IO_LOC "clk" 52;
IO_PORT "clk" IO_TYPE=LVCMOS33 PULL_MODE=UP;

IO_LOC "key_reset_n" 3;
IO_PORT "key_reset_n" IO_TYPE=LVCMOS18 PULL_MODE=UP;

IO_LOC "key_enable_n" 4;
IO_PORT "key_enable_n" IO_TYPE=LVCMOS18 PULL_MODE=UP;

IO_LOC "led_n" 16;
IO_PORT "led_n" IO_TYPE=LVCMOS33;
```

Si el nombre no coincide, el toolchain no puede asignar ese puerto a un pin fisico.

## Líneas Principales

Cada señal normalmente usa dos líneas:

```text
IO_LOC "nombre_señal" número_pin;
IO_PORT "nombre_señal" IO_TYPE=LVCMOS33;
```

- `IO_LOC`: asigna una señal a un pin físico.
- `IO_PORT`: configura características eléctricas del pin.
- `IO_TYPE`: define el voltaje/interfaz eléctrica.
- `PULL_MODE=UP`: habilita resistencia pull-up interna, útil para botones o entradas abiertas.

## Entradas

Para entradas con boton o GPIO con pull-up:

```text
IO_LOC "gpio3_n" 3;
IO_PORT "gpio3_n" IO_TYPE=LVCMOS18 PULL_MODE=UP;
```

En el HDL se declara como entrada:

```verilog
input wire gpio3_n
```

El sufijo `_n` indica que la señal es activa en bajo. Con pull-up, la entrada normalmente vale `1`; cuando presionas el botón o conectas a tierra, vale `0`.

Para convertirla a lógica positiva:

```verilog
wire gpio3 = ~gpio3_n;
```

## Salidas

Para una salida de LED:

```text
IO_LOC "led_n" 16;
IO_PORT "led_n" IO_TYPE=LVCMOS33;
```

En el HDL:

```verilog
output wire led_n
```

Si el LED es activo en bajo, se enciende con `0` y se apaga con `1`:

```verilog
assign led_n = ~resultado;
```

## Reloj

El reloj principal de estas prácticas usa el pin 52:

```text
IO_LOC "clk" 52;
IO_PORT "clk" IO_TYPE=LVCMOS33;
```

En Verilog:

```verilog
input wire clk
```

En VHDL:

```vhdl
clk : in std_logic
```

Usa `clk` solo para lógica secuencial: contadores, registros y máquinas de estado.

## Ejemplo Completo

Este ejemplo define dos entradas y una salida:

```text
IO_LOC "key_reset_n" 3;
IO_PORT "key_reset_n" IO_TYPE=LVCMOS18 PULL_MODE=UP;

IO_LOC "key_enable_n" 4;
IO_PORT "key_enable_n" IO_TYPE=LVCMOS18 PULL_MODE=UP;

IO_LOC "led_n" 16;
IO_PORT "led_n" IO_TYPE=LVCMOS33;
```

Top equivalente en Verilog:

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

Top equivalente en VHDL:

```vhdl
library ieee;
use ieee.std_logic_1164.all;

entity top is
    port (
        key_reset_n  : in std_logic;
        key_enable_n : in std_logic;
        led_n        : out std_logic
    );
end entity;

architecture rtl of top is
    signal a : std_logic;
    signal b : std_logic;
begin
    a <= not key_reset_n;
    b <= not key_enable_n;
    led_n <= not (a and b);
end architecture;
```

## Checklist Para Crear un CST

1. Escribe primero los puertos de tu `top`.
2. Usa esos mismos nombres en `IO_LOC` e `IO_PORT`.
3. Consulta el pin físico correcto del target.
4. Usa `IO_TYPE` compatible con el banco de voltaje del pin.
5. Agrega `PULL_MODE=UP` en entradas que deben quedar en `1` cuando están libres.
6. Marca señales activas en bajo con sufijo `_n`.
7. Declara el archivo en `devlab.toml` con `constraints = "pins.cst"`.

## Errores Comunes

- El puerto existe en HDL pero no en `pins.cst`.
- El nombre en `pins.cst` no coincide exactamente con el puerto.
- Se usa un pin equivocado o reservado.
- Se olvida la polaridad activa en bajo del LED o boton.
- Se usa `LVCMOS33` en una entrada que en el target esta conectada a banco de `1.8 V`.

Para las practicas de este repositorio, toma como referencia los `pins.cst` ya incluidos en cada ejemplo.
