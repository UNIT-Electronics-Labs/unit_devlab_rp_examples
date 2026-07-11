# Blink: Parpadeo del LED integrado

Esta práctica introduce el control del periférico GPIO (*General Purpose Input/Output*) del RP2040 en su modalidad de salida digital, mediante el parpadeo del LED integrado en la placa. El control de salidas digitales es la operación más elemental de cualquier sistema embebido: encender un indicador, accionar un relevador, manejar un transistor de potencia o generar una señal de control hacia otro circuito parten, en todos los casos, de esta misma operación básica.

## Concepto Teórico

A nivel de silicio, cada pin GPIO del RP2040 se conecta, a través de un bloque de control de pad, a un multiplexor de funciones (*IO Bank0*) que permite asignarle una de varias funciones posibles (UART, I2C, SPI, PWM, o entrada/salida de propósito general). Cuando un pin se configura para entrada/salida de propósito general, su estado queda gobernado por el bloque SIO (*Single-cycle IO*), un periférico de acceso directo pensado para operaciones GPIO de baja latencia: el registro `GPIO_OE` determina si el pin actúa como entrada o como salida, y el registro `GPIO_OUT` determina, bit a bit, el nivel lógico de cada salida.

Una particularidad del RP2040, relevante para el resto del curso, es que estos registros exponen alias de acceso atómico (`_SET`, `_CLR`, `_XOR`) que permiten modificar un único bit sin necesidad de leer, modificar y volver a escribir el registro completo, evitando con ello condiciones de carrera cuando distintas partes de un programa manipulan pines diferentes. Las funciones `gpio_set_dir` y `gpio_put` del SDK son, en última instancia, envolturas sobre estos alias atómicos.

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
 * @file Practice_Blink_01.c
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

    while (1) {
        gpio_put(LED_PIN, 1);   // Enciende el LED
        sleep_ms(1000);         // Retardo de 1 segundo
        gpio_put(LED_PIN, 0);   // Apaga el LED
        sleep_ms(1000);         // Retardo de 1 segundo
    }
}
```

## Análisis del Código

`gpio_init` restablece el pin a un estado conocido y lo configura para la función SIO (entrada/salida de propósito general), en lugar de cualquier otra función alternativa disponible en ese pin. `gpio_set_dir` escribe el bit correspondiente del registro `GPIO_OE`, configurando el pin como salida. `gpio_put` escribe, mediante los alias atómicos descritos en la sección anterior, el bit correspondiente del registro `GPIO_OUT`, fijando el nivel lógico del LED. `sleep_ms` introduce un retardo bloqueante basado en el temporizador de hardware del RP2040, empleado aquí únicamente para que el parpadeo resulte perceptible al ojo humano.

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
