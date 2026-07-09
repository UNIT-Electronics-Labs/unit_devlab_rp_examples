# Ejemplo 03: Lectura de temperatura mediante ADC

Corresponde a la práctica **Lectura de temperatura mediante ADC** del Módulo II. Usa el sensor de temperatura interno del RP2040 (conectado internamente al canal 4 del ADC) — no requiere ningún componente externo, lo que lo hace un buen segundo ejemplo antes de pasar a sensores analógicos reales.

## Conexiones

Ninguna. El sensor es interno al chip.

## Estructura del proyecto

```
03_lectura_temperatura_adc/
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── main.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(lectura_temperatura C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(lectura_temperatura
    main.c
)

target_link_libraries(lectura_temperatura
    pico_stdlib
    hardware_adc
)

pico_add_extra_outputs(lectura_temperatura)
pico_enable_stdio_usb(lectura_temperatura 1)
pico_enable_stdio_uart(lectura_temperatura 0)
```

## main.c

```c
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

int main() {
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);  // canal 4 = sensor de temperatura interno

    while (true) {
        uint16_t raw = adc_read();
        float voltage = raw * 3.3f / 4096.0f;
        float temp = 27.0f - (voltage - 0.706f) / 0.001721f;

        printf("Temperatura: %.2f C\n", temp);
        sleep_ms(1000);
    }
}
```

## Explicación

El ADC del RP2040 es de 12 bits (0–4095) sobre una referencia de 3.3 V; `raw * 3.3f / 4096.0f` convierte la lectura cruda a voltaje. La fórmula de temperatura es la aproximación lineal que da el fabricante a partir del voltaje del sensor interno — es útil para detectar cambios relativos (¿subió o bajó?), pero no es un sensor de precisión: para valores absolutos confiables se necesita un sensor externo calibrado (LM35, DS18B20, etc.) conectado a uno de los canales 0–3. El detalle de `adc_init`/`adc_select_input` está en la [guía de C, sección ADC](../c-programming.md#adc-hardware_adc).

## Compilar y cargar

```bash
mkdir build && cd build
cmake ..
make
```

Ver [configuración del entorno](../devlab.md#cargar-el-programa) para el flasheo.

## Verificación

Con una terminal serial sobre el puerto USB-CDC de la placa a 115200 baudios, debe imprimirse una lectura de temperatura cada segundo, cercana a la temperatura ambiente (con algo de desviación — normal en el sensor interno).

## Variantes

- Promediar 10 lecturas antes de imprimir, para reducir el ruido.
- Migrar a un sensor analógico externo en el canal 0, 1 o 2, y comparar la estabilidad de la lectura contra el sensor interno.
- Encender el LED del [ejemplo 00](./00-parpadeo-led.md) si la temperatura supera un umbral.

## Errores comunes

| Síntoma | Causa típica |
|---|---|
| Siempre lee un valor fijo o absurdo | Falta `adc_set_temp_sensor_enabled(true)` antes de seleccionar el canal 4 |
| No se ve nada en la terminal | `pico_enable_stdio_usb` no está en `1` |
| Falla el *linking* | Falta `hardware_adc` en `target_link_libraries` |
