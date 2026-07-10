# Lectura Analógica del TMP235 mediante ADC (Pico SDK)

Constituye una extensión avanzada de la práctica **Lectura de temperatura mediante ADC** del Módulo II. A diferencia del ejemplo básico, que utiliza el sensor de temperatura interno del RP2040, este ejemplo introduce un sensor analógico externo (TMP235) conectado a uno de los canales físicos del ADC, lo que permite abordar el cableado de un transductor real y la interpretación de su función de transferencia.

## Objetivo

Leer el voltaje de salida del sensor TMP235 mediante el canal ADC0 del RP2040 (GPIO26) y convertirlo a una lectura de temperatura en grados Celsius, aplicando la función de transferencia lineal del fabricante.

## Hardware

| Componente | Cantidad |
|---|---|
| Raspberry Pi Pico (RP2040) | 1 |
| Sensor TMP235 (empaque SOT-23) | 1 |
| Cables de conexión | 3 |

## Conexiones

El TMP235 en empaque SOT-23 asigna los pines de la siguiente manera:

| Pin del TMP235 | Señal | Conexión en la Pico |
|---|---|---|
| 1 | VDD | 3V3(OUT) |
| 2 | VOUT | GPIO26 (ADC0) |
| 3 | GND | GND |

> Si el sensor proviene de una placa breakout con pines serigrafiados, debe verificarse el orden ahí directamente, ya que no todas las breakouts respetan el orden crudo del empaque SOT-23.

## Función de transferencia

El TMP235 entrega un voltaje proporcional a la temperatura, válido en el rango de −40 °C a 100 °C:

```
Vout = 500 mV + 10 mV/°C × T
```

Despejando T:

```
T (°C) = (Vout_mV − 500) / 10
```

## Estructura del proyecto

```
lectura_tmp235_adc/
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── main.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(lectura_tmp235_adc C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(lectura_tmp235_adc
    main.c
)

target_link_libraries(lectura_tmp235_adc
    pico_stdlib
    hardware_adc
)

pico_add_extra_outputs(lectura_tmp235_adc)
pico_enable_stdio_usb(lectura_tmp235_adc 1)
pico_enable_stdio_uart(lectura_tmp235_adc 0)
```

## main.c

```c
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

int main() {
    stdio_init_all();

    adc_init();
    adc_gpio_init(26);       // Deshabilita la función digital en GPIO26
    adc_select_input(0);     // GPIO26 corresponde al canal ADC0

    while (true) {
        uint16_t raw = adc_read();                      // 0 a 4095
        float voltage_mV = raw * 3300.0f / 4096.0f;      // 3.3 V = 3300 mV
        float temp_c = (voltage_mV - 500.0f) / 10.0f;    // Función de transferencia

        printf("raw=%u  V=%.1f mV  T=%.2f C\n", raw, voltage_mV, temp_c);
        sleep_ms(500);
    }
}
```

## Explicación

`adc_gpio_init(26)` deshabilita las funciones digitales (entrada/salida y pull-up/pull-down) del pin, dejándolo en alta impedancia para uso analógico. `adc_select_input(0)` enruta el canal correspondiente a GPIO26 hacia el convertidor. `adc_read()` bloquea hasta completar la conversión de 12 bits y devuelve un valor entre 0 y 4095, proporcional al voltaje presente en el pin sobre una referencia nominal de 3.3 V.

## Verificación

Debe observarse en la terminal serie un valor de voltaje cercano a 500-800 mV a temperatura ambiente, correspondiente a una temperatura razonable (20-30 °C). Valores de voltaje cercanos a 3.3 V (o `raw` cercano a 4095) no son físicamente posibles para este sensor e indican un problema de cableado.

## Errores comunes

| Síntoma | Causa típica |
|---|---|
| Voltaje leído cercano a 3.3 V y temperatura fuera de rango físico (superior a 150 °C) | GND del sensor sin conexión común con la Pico, o VDD/VOUT invertidos |
| `raw` constante en 0 | VOUT sin conexión, o cortocircuito a GND |
| Falla en el *linking* | Falta `hardware_adc` en `target_link_libraries` |
| Lectura ruidosa o inestable | Ausencia de capacitor de desacople (0.1 µF) entre VDD y GND del sensor |

## Variantes

- Sustituir la impresión por consola por una visualización en pantalla OLED (documentado por separado).
- Promediar N lecturas consecutivas para reducir el ruido de cuantización.
- Comparar la lectura contra el sensor de temperatura interno del RP2040 (canal 4) para verificar consistencia relativa entre ambos.
