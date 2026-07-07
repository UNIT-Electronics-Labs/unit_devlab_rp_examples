# Introducción a VHDL

VHDL también describe hardware. Es más explícito que Verilog: separa la interfaz (`entity`) de la implementación (`architecture`) y usa tipos fuertes.

## Entity

La entidad define los puertos del circuito.

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
```

## Architecture

La arquitectura implementa el circuito.

```vhdl
architecture rtl of top is
    signal a : std_logic;
    signal b : std_logic;
begin
    a <= not key_reset_n;
    b <= not key_enable_n;
    led_n <= not (a and b);
end architecture;
```

## Señales y Tipos

- `std_logic`: bit con estados eléctricos.
- `unsigned`: vector numérico sin signo, usado para sumadores y contadores.
- `signal`: conexión interna de hardware.
- `process`: bloque para lógica secuencial o combinacional estructurada.

## Proceso con Reloj

```vhdl
process(clk)
begin
    if rising_edge(clk) then
        if key_reset_n = '0' then
            counter <= (others => '0');
        else
            counter <= counter + 1;
        end if;
    end if;
end process;
```

## Cómo Usar VHDL en Este Repo

Cada práctica tiene un `src/top.vhd` autocontenido y un archivo `devlab-vhdl.toml`.

```bash
cd digital-labs/01_sumador_n_bits
devlab build -c devlab-vhdl.toml
devlab flash
```

El flujo por defecto usa Verilog porque suele ser más portable en entornos donde el plugin GHDL de Yosys no está disponible. VHDL se conserva como alternativa didáctica y se compila de forma explícita.
