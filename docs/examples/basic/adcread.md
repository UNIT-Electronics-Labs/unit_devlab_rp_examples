# Ejemplo 03: Lectura de temperatura mediante ADC

Esta práctica corresponde al bloque de conversión analógico-digital del Módulo II y se compone de dos partes secuenciales. La primera introduce la configuración del periférico ADC del RP2040 mediante el sensor de temperatura integrado en el propio microcontrolador, sin requerir componentes adicionales. La segunda extiende dicha configuración a la lectura de un sensor de temperatura externo (TMP235), lo que permite contrastar el comportamiento de una referencia interna frente a un transductor analógico real conectado a uno de los canales disponibles del ADC.
 
La secuencia busca que el alumno comprenda primero la configuración mínima del periférico — inicialización, selección de canal y conversión— antes de introducir las consideraciones adicionales que implica el acondicionamiento de una señal externa: configuración del pin como entrada analógica, alimentación del sensor y validez del rango de conversión.

## Objetivo general
 
Configurar y emplear el convertidor analógico-digital (ADC) del RP2040 para la adquisición de lecturas de temperatura, utilizando en la primera parte el sensor interno del microcontrolador y, en la segunda, un sensor de temperatura analógico externo (TMP235), con el fin de establecer una comparación entre ambas fuentes de medición.

### Hardware requerido (sensor interno)
 
| Componente | Cantidad | Observaciones |
|---|---|---|
| Placa con RP2040 (Raspberry Pi Pico o equivalente) | 1 | Plataforma empleada en el Módulo II |
| UNIT DevLab MultiHub Shield | 1 | Plataforma empleada en el Módulo II |
 
### Conexiones (sensor interno)
 
No se requiere ninguna conexión externa. El sensor de temperatura se encuentra integrado en el propio microcontrolador y su lectura se realiza internamente a través del canal 4 del ADC.

### Estructura del proyecto (sensor interno)
 
```
ADCPractice_02/
├── build
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── ADCPractice_02.c
```
 
### CMakeLists.txt (sensor interno)

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por pico-new
# ─────────────────────────────────────────────
set(PROJECT_NAME    "ADCPractice_02")
set(PROJECT_SOURCES "ADCPractice_02.c")
set(PICO_BOARD      "pico")

set(PROJECT_LIBS
    pico_stdlib
    hardware_adc
)
# ─────────────────────────────────────────────
# NO MODIFICAR DE AQUÍ EN ADELANTE
# ─────────────────────────────────────────────

project(${PROJECT_NAME})

pico_sdk_init()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
```

### Código fuente (sensor interno) — `ADCPractice_02.c`

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

### Explicación (sensor interno)
 
El ADC del RP2040 dispone de una resolución de 12 bits (0–4095) sobre una referencia de 3.3 V, de manera que la conversión de la lectura cruda a voltaje se obtiene mediante `raw * 3.3f / 4096.0f`. La fórmula de temperatura corresponde a la aproximación lineal proporcionada por el fabricante a partir de la tensión del sensor interno. Dicha aproximación resulta adecuada para detectar variaciones relativas de temperatura, pero no constituye una medición de precisión, ya que el sensor interno no se calibra individualmente en fábrica. Para obtener valores absolutos confiables se recomienda el uso de un sensor externo calibrado, como el que se emplea en la segunda parte de esta práctica. El detalle de las funciones `adc_init` y `adc_select_input` se documenta en la [guía de C, sección ADC](../../guide/c-programming.md#adc-hardware_adc).

### Errores comunes (sensor interno)
 
| Síntoma | Causa típica |
|---|---|
| Se obtiene siempre un valor fijo o carente de sentido | No se invocó `adc_set_temp_sensor_enabled(true)` antes de seleccionar el canal 4 |
| No se observa salida alguna en la terminal | `pico_enable_stdio_usb` no fue habilitado con el valor `1` en el CMakeLists |
| Error de *linking* durante la compilación | Ausencia de `hardware_adc` en `target_link_libraries` |
 
---

## Segunda parte: sensor de temperatura externo (TMP235)

### Hardware requerido (sensor externo)
 
| Componente | Cantidad | Observaciones |
|---|---|---|
| Placa con RP2040 (Raspberry Pi Pico o equivalente) | 1 | Plataforma empleada en el Módulo II |
| Sensor de temperatura TMP235 | 1 | Salida analógica lineal de 10 mV/°C, encapsulado SOT-23 |
| Cables de conexión (jumper) | 3 | Para las señales VS, OUT y GND |

### Conexiones (sensor externo)
 
| Señal del sensor | Pin del RP2040 | Descripción |
|---|---|---|
| VS (alimentación) | 3V3(OUT) | El TMP235 admite de 2.3 V a 5.5 V; la salida de 3.3 V de la placa es adecuada |
| OUT (salida analógica) | GPIO26 / ADC0 | Señal analógica proporcional a la temperatura, con pendiente de 10 mV/°C |
| GND | GND | Referencia común de tierra entre el sensor y la placa |
 
> **Nota:** conforme a la hoja de datos del fabricante, el uso de un capacitor de desacoplo de 0.1 µF entre VS y GND es opcional, pero se recomienda en entornos con ruido eléctrico considerable.

### Estructura del proyecto (sensor externo)
 
```
ADCExternalPractice_03/
├── build
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── ADCExternalPractice_03.c
```

### CMakeLists.txt (sensor externo)

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por pico-new
# ─────────────────────────────────────────────
set(PROJECT_NAME    "ADCExternalPractice_03")
set(PROJECT_SOURCES "ADCExternalPractice_03.c")
set(PICO_BOARD      "pico")

set(PROJECT_LIBS
    pico_stdlib
    hardware_adc
)
# ─────────────────────────────────────────────
# NO MODIFICAR DE AQUÍ EN ADELANTE
# ─────────────────────────────────────────────

project(${PROJECT_NAME})

pico_sdk_init()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
```


### Código fuente (sensor externo) — `ADCExternalPractice_03.c`

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
        float temp =(voltage - 0.5f) * 100;

        printf("Temperatura: %.2f C\n", temp);
        sleep_ms(1000);
    }
}
```
### Explicación (sensor externo)
 
A diferencia del sensor interno, un canal de ADC conectado a un pin físico requiere que dicho pin se configure explícitamente como entrada analógica mediante `adc_gpio_init()`; esta llamada deshabilita las resistencias de *pull-up*/*pull-down* y el buffer digital del pin, condición necesaria para obtener una lectura analógica estable. El TMP235 entrega una tensión de salida linealmente proporcional a la temperatura, con un valor de referencia de 500 mV a 0 °C y una pendiente de 10 mV/°C; de ahí que la temperatura se obtenga despejando dicha relación: `(voltage - 0.5) / 0.01`, equivalente a `(voltage - 0.5) * 100`. Esta fórmula es válida en el rango de −40 °C a 150 °C que especifica el fabricante.
 
### Errores comunes (sensor externo)
 
| Síntoma | Causa típica |
|---|---|
| Lectura inestable o con ruido excesivo | No se invocó `adc_gpio_init()` antes de seleccionar el canal |
| Lectura fija cercana a un valor incorrecto | Se seleccionó por error el canal 4 (sensor interno) en lugar del canal 0 (GPIO26) |
| Temperatura leída muy por debajo de la ambiente | Conexión invertida entre VS y GND, o sensor sin alimentación |
| Error de *linking* durante la compilación | Ausencia de `hardware_adc` en `target_link_libraries` |
 
---

## Compilación y carga
Ambas partes de la práctica se compilan y cargan de la misma manera, sustituyendo el nombre del proyecto según corresponda (`ADCPractice_02` o `ADCExternalPractice_03`):
 
```bash
pico-flash
```
 
Consúltese la [configuración del entorno](../../guide/devlab.md#cargar-el-programa) para el procedimiento de flasheo.

## Verificación
 
La verificación de ambas partes se realiza mediante una terminal serial sobre el puerto USB-CDC de la placa, a 115200 baudios, habilitado mediante la instrucción `pico_enable_stdio_usb(${PROJECT_NAME} 1)` del CMakeLists:
 
```bash
minicom -b 115200 -D /dev/ttyACM0
```
 
En ambos casos debe observarse una lectura de temperatura impresa cada segundo. En la parte del sensor interno, el valor debe aproximarse a la temperatura ambiente, con cierta desviación característica del sensor integrado; en la parte del TMP235, la lectura debe ser más estable y responder de forma más inmediata a cambios de temperatura provocados de manera controlada (por ejemplo, al sostener el sensor entre los dedos).
 
<div align="center">
  <img src="../resources/adcpractice3.png" width="300px" alt="Lectura de temperatura en terminal serial">
  <p><em>Salida esperada en la terminal serial</em></p>
</div>

## Variantes
 
- Promediar 10 lecturas consecutivas antes de imprimir el resultado, con el fin de reducir el ruido de conversión.
- Leer ambos sensores (interno y externo) de manera simultánea, alternando entre canales mediante `adc_select_input()`, y comparar la estabilidad y velocidad de respuesta de cada uno.
- Encender el LED del [ejemplo 00](./blink.md) cuando la temperatura leída por el TMP235 supere un umbral definido.
- Calibrar el desplazamiento (*offset*) del TMP235 comparando la lectura contra un termómetro de referencia, y ajustar la constante de 0.5 V si se observa una desviación sistemática.
 