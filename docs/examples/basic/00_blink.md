# Blink: Parpadeo del LED integrado

Esta práctica introduce el control del periférico GPIO (*General Purpose Input/Output*) del RP2040 en su modalidad de salida digital, mediante el parpadeo del LED integrado en la placa. El control de salidas digitales es la operación más elemental de cualquier sistema embebido: encender un indicador, accionar un relevador, manejar un transistor de potencia o generar una señal de control hacia otro circuito parten, en todos los casos, de esta misma operación básica.

## Concepto Teórico

A nivel de silicio, cada pin GPIO del RP2040 se conecta, a través de un bloque de control de pad, a un multiplexor de funciones que permite asignarle una de varias funciones posibles (UART, I2C, SPI, PWM, o entrada/salida de propósito general). Cuando un pin se configura para entrada/salida de propósito general, su estado queda gobernado por un bloque de acceso directo de baja latencia; el SDK garantiza que fijar o limpiar el pin sea una operación atómica, sin necesidad de leer, modificar y volver a escribir el estado completo del puerto, evitando así condiciones de carrera cuando distintas partes de un programa manipulan pines diferentes.
 
Más allá de encender o apagar el pin, el bloque de control de pad de cada GPIO permite además ajustar sus características eléctricas de salida: la corriente máxima que puede entregar (*drive strength*) y la velocidad con la que transiciona entre niveles lógicos (*slew rate*). Una corriente de manejo mayor permite alimentar cargas más demandantes, mientras que una transición más lenta reduce el ruido electromagnético generado, a costa de una conmutación menos abrupta. Ninguno de los dos resulta indispensable para un LED simple, por lo que en esta práctica se dejan documentados pero no activados.

## Hardware y Conexiones

No se requieren componentes adicionales: se utiliza el LED integrado en la placa, montada sobre el UNIT DevLab MultiHub Shield que sirve como base física a lo largo de las prácticas del curso.
 
| Elemento | Pin del RP2040 | Descripción |
|---|---|---|
| LED integrado | GPIO25 | LED montado en la placa; no requiere conexión externa |

## Configuración del Proyecto (CMake)

Se requiere enlazar la librería `hardware_gpio`, además de `pico_stdlib`:

```cmake
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_gpio
)
```

## Código Fuente

```c
/**
 * @file main.c
 * @brief Parpadeo del LED integrado del RP2040
 *
 * @author obviousfancy
 * @board  pico
 * @sdk    Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* ─── Defines ──────────────────────────────────────────── */
#define LED_PIN 25

/* ─── Main ─────────────────────────────────────────────── */
int main() {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    // El pin admite ajustar su corriente maxima de salida (drive strength,
    // de 2 a 12 mA) y la velocidad de transicion entre niveles logicos
    // (slew rate). No son necesarias para un LED simple; ejemplo de uso
    // si se necesitaran para una carga mas demandante o para reducir
    // ruido electromagnetico:
    // gpio_set_drive_strength(LED_PIN, GPIO_DRIVE_STRENGTH_4MA);
    // gpio_set_slew_rate(LED_PIN, GPIO_SLEW_RATE_SLOW);

    while (1) {
        gpio_put(LED_PIN, 1);   // Enciende el LED
        sleep_ms(1000);         // Retardo de 1 segundo
        gpio_put(LED_PIN, 0);   // Apaga el LED
        sleep_ms(1000);         // Retardo de 1 segundo
    }
}
```

## Análisis del Código

`gpio_init` restablece el pin a un estado conocido y lo configura para la función de entrada/salida de propósito general, en lugar de cualquier otra función alternativa disponible en ese pin. `gpio_set_dir` configura el pin como salida. Las líneas comentadas muestran cómo se ajustarían `gpio_set_drive_strength` (corriente máxima que el pin puede entregar, hasta 12 mA) y `gpio_set_slew_rate` (velocidad de transición eléctrica) si la carga conectada lo requiriera; para el LED de esta práctica, los valores por defecto del pin son suficientes. `gpio_put` fija el nivel lógico del LED en cada iteración. `sleep_ms` introduce un retardo bloqueante basado en el temporizador de hardware del RP2040, empleado aquí únicamente para que el parpadeo resulte perceptible al ojo humano.

## Verificación

El LED integrado debe encender y apagar a intervalos de un segundo. Esta práctica no emplea comunicación serial: la inicialización de `stdio` y el uso de `printf` se introducen en la práctica siguiente (Serial).

## Errores Comunes y Variantes

| Síntoma | Causa típica |
|---|---|
| Falla la compilación: `PICO_BOARD` no reconocido | El archivo `pico_sdk_import.cmake` no fue copiado al directorio del proyecto |
| Compila correctamente pero no se generan los archivos necesarios para el flasheo | Ausencia de `pico_add_extra_outputs()` en el CMakeLists.txt |
| El LED no enciende en ningún momento tras el flasheo | Conexión defectuosa del depurador SWD (líneas SWCLK/SWDIO) entre el CH552 y la placa, o fallo de comunicación reportado por OpenOCD o PyOCD (según el caso) |

**Variantes:**

- Cambiar el patrón a un parpadeo tipo ["SOS" (código Morse)](../../examples/basic/variantes.md#gpio).
- Hacer que el tiempo de encendido y el de apagado sean distintos entre sí (por ejemplo, encendido breve y apagado prolongado).
- Agregar un segundo LED externo en otro pin y alternar entre ambos.
