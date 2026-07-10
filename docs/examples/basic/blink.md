# Ejemplo 01: LED Blinking

La presente práctica constituye el primer contacto formal con la plataforma RP2040 dentro del entorno de desarrollo basado en el SDK oficial de Raspberry Pi. Su finalidad consiste en validar de manera íntegra la cadena de herramientas de desarrollo —compilador ARM GNU, sistema de compilación CMake y bibliotecas del SDK— mediante el control del periférico GPIO en su expresión más elemental: el encendido y apagado periódico del LED integrado en la placa. Esta validación resulta indispensable antes de abordar los periféricos de mayor complejidad que se presentan en las prácticas subsecuentes del Módulo II, ya que permite garantizar que cualquier incidencia observada en ellas corresponda a la lógica del programa y no a una deficiencia en la configuración del entorno de desarrollo.

## Objetivo general
 
Configurar y controlar un pin GPIO del RP2040 como salida digital mediante el SDK oficial, validando de esta manera el flujo completo de compilación, enlazado y carga de un programa sobre la placa.

## Hardware requerido
 
| Componente | Cantidad | Observaciones |
|---|---|---|
| Placa con RP2040 (Raspberry Pi Pico o equivalente) | 1 | Se emplea el LED integrado en la placa; no se requiere hardware adicional |
| UNIT DevLab MultiHub Shield | 1 | |

# Conexiones
 
| Señal | Pin del RP2040 | Descripción |
|---|---|---|
| LED integrado | GPIO25 | LED montado en la placa |

## Estructura del proyecto
 
```
Practice_Blink_01/
├── build
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── Practice_Blink_01.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por pico-new
# ─────────────────────────────────────────────
set(PROJECT_NAME    "Practice_Blink_01")
set(PROJECT_SOURCES "Practice_Blink_01.c")
set(PICO_BOARD      "pico")

set(PROJECT_LIBS
    pico_stdlib
    hardware_gpio
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

::: info 
No enlaza ninguna librería `hardware_*` de forma nativa, porque `pico_stdlib` ya contiene GPIO básico y `sleep_ms`, pero si queremos tener de manera verbose 
las funcionalidades utilizadas podemos agregar hardware/gpio.h
:::

## Código fuente — `Practice_Blink_01.c`

```c
/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* ─── Defines ──────────────────────────────────────────── */
#define LED_PIN 25

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        // TODO: lógica principal del proyecto
        gpio_put(LED_PIN, 1); // Turn on LED
        printf("LED ON\n");
        sleep_ms(1000); // Delay for 1 second
        gpio_put(LED_PIN, 0); // Turn off LED
        printf("LED OFF\n");
        sleep_ms(1000); // Delay for 1 second
    }
}

```

## Explicación
 
Tres llamadas concentran la totalidad de la lógica: `gpio_init` reserva el pin y lo inicializa, `gpio_set_dir` lo configura como salida digital, y el ciclo alterna el estado del pin mediante `gpio_put` cada 1000 ms. Aunque `pico_stdlib` ya integra las funciones básicas de GPIO y de temporización (`sleep_ms`), en el CMakeLists se enlaza explícitamente la biblioteca `hardware_gpio`, con el fin de que el estudiante identifique con claridad qué biblioteca provee cada función empleada; este criterio se mantiene a lo largo de todos los ejemplos del curso. El detalle de cada función se documenta en la [guía de C, sección GPIO](../../guide/c-programming.md#gpio-pico_stdlib).

## Errores comunes
 
| Síntoma | Causa típica |
|---|---|
| Falla la compilación: `PICO_BOARD` no reconocido | El archivo `pico_sdk_import.cmake` no fue copiado al directorio del proyecto |
| Compila correctamente pero no se generan los archivos necesarios para el flasheo | Ausencia de la instrucción `pico_add_extra_outputs()` en el CMakeLists.txt |
| El LED no enciende en ningún momento tras el flasheo | Conexión defectuosa del depurador SWD (líneas SWCLK/SWDIO) entre el CH552 y la placa, o fallo de comunicación reportado por OpenOCD o PyOCD(dependiendo del caso) |

## Compilar y cargar

## Compilación y carga
 
La programación de la placa se realiza mediante el depurador CMSIS-DAP basado en el microcontrolador CH552, a través del protocolo SWD (Serial Wire Debug), y no mediante el modo de almacenamiento masivo (BOOTSEL) característico de otros flujos de trabajo con el RP2040. El script `pico-flash` invoca a OpenOCD para transferir el binario directamente a la memoria flash del microcontrolador y reiniciar su ejecución:
 
```bash
pico-flash
```
 
Consúltese la [configuración del entorno](../../guide/devlab.md#cargar-el-programa) para el detalle de la configuración del depurador y del procedimiento de flasheo.
 
## Verificación
 
El LED integrado debe encender y apagar a intervalos de un segundo. De manera simultánea, el comportamiento puede verificarse mediante una terminal serial sobre el puerto USB-CDC de la placa, a 115200 baudios, habilitado mediante `pico_enable_stdio_usb(${PROJECT_NAME} 1)`:
 
```bash
minicom -b 115200 -D /dev/ttyACM0
```
 
Deben observarse los mensajes `LED ON` y `LED OFF` alternándose cada segundo, en sincronía con el parpadeo físico del LED.
 
<div align="center">
  <img src="../resources/blinkpractice1.png" width="300px" alt="Mensajes LED ENCENDIDO y LED APAGADO en terminal serial">
  <p><em>Salida esperada en la terminal serial</em></p>
</div>

Si no se observa actividad en el LED, se recomienda verificar en primer lugar que el script `pico-flash` haya reportado una programación exitosa a través de OpenOCD, antes de revisar el código fuente. Entre las causas más frecuentes de falla se encuentran una conexión defectuosa del depurador SWD (líneas SWCLK y SWDIO) entre el CH552 y la placa RP2040, o la falta de alimentación del microcontrolador objetivo.
 
## Variantes
 
- Cambiar el patrón a un parpadeo tipo "SOS" (código Morse).
- Controlar la frecuencia de parpadeo mediante un GPIO de entrada (véase el [ejemplo 01](./gpio.md)).
- Agregar un segundo LED externo en otro pin y alternar entre ambos.
 