# Serial: Comunicación por USB (stdio)

Esta práctica introduce la comunicación serial del RP2040 a través del puerto USB, empleando la capa estándar de entrada/salida de C (`stdio`) para enviar mensajes desde la placa hacia una computadora. Esta forma de comunicación es la que se emplea, a lo largo de todo el curso a excepcion de la practica de UART, para observar el comportamiento interno de un programa mediante mensajes de diagnóstico —una herramienta imprescindible en cualquier trabajo de desarrollo embebido, donde no siempre se dispone de una pantalla conectada al propio microcontrolador.

## Concepto Teórico

El RP2040 no dispone de un controlador de puerto serial "virtual" dedicado: lo que en esta práctica se percibe como un puerto serial sobre USB es, en realidad, una implementación de la clase USB CDC-ACM (*Communications Device Class*) que corre sobre el controlador USB nativo del RP2040, y que el sistema operativo del equipo host reconoce como un puerto serial virtual (por ejemplo, `/dev/ttyACMx` en Linux). Esto la distingue de una UART de hardware real —periférico dedicado con líneas físicas TX/RX, que se aborda más adelante en el curso— aunque, desde el punto de vista del programador, ambas interfaces se manejan mediante la misma capa estándar de E/S (`stdio`).

La selección de qué interfaz alimenta esa capa de `stdio` no ocurre en el código en C, sino en tiempo de configuración de CMake, mediante `pico_enable_stdio_usb()` y `pico_enable_stdio_uart()`. En tiempo de ejecución, `stdio_init_all()` simplemente inicializa todas las interfaces que hayan sido habilitadas de esta manera. Por ello, `printf` no necesita indicar hacia qué canal escribe: su destino quedó determinado desde la compilación del proyecto.

## Hardware y Conexiones

| Elemento | Pin del RP2040 | Descripción |
|---|---|---|
| Puerto USB | — | Se utiliza el mismo cable USB empleado para alimentar y programar la placa; no se emplea ningún pin GPIO en esta práctica |

## Configuración del Proyecto (CMake)

No se requiere ninguna librería `hardware_*` adicional; basta con `pico_stdlib`. El elemento indispensable para esta práctica se configura mediante otras dos instrucciones ya presentes en la plantilla generada por `pico-new` —`pico_enable_stdio_usb(${PROJECT_NAME} 1)` y `pico_enable_stdio_uart(${PROJECT_NAME} 0)`— que no requieren modificarse.

```cmake
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
)
```

## Código Fuente

```c
/**
 * @file main.c
 * @brief Comunicacion serial basica sobre USB (stdio)
 *
 * @author obviousfancy
 * @board  pico
 * @sdk    Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();

    uint32_t contador = 0;

    while (1) {
        printf("Contador: %lu\n", contador);
        contador++;
        sleep_ms(1000);
    }
}
```

## Análisis del Código

`stdio_init_all()` inicializa todas las interfaces de `stdio` habilitadas desde el CMakeLists —en este caso, únicamente USB—, dejándolas listas para recibir datos de `printf`. La variable `contador` existe únicamente para que cada mensaje impreso sea distinto del anterior, permitiendo comprobar a simple vista que el programa continúa ejecutándose y que ningún mensaje se repite o se pierde. `sleep_ms(1000)`, ya utilizado en la práctica de Blink, controla el intervalo entre mensajes sucesivos.

## Verificación

Ábrase una terminal serial de su preferencia sobre el puerto USB-CDC que expone la placa (por ejemplo, `/dev/ttyACM0` en Linux) a 115200 baudios, en linux puede usarse:

```bash
minicom -b 115200 -D /dev/ttyACM0
```

Debe observarse una línea `Contador: N` impresa cada segundo, con `N` incrementándose de manera consecutiva y sin saltos ni repeticiones.

<div align="center">
  <img src="../resources/02_stdio.png" width="300px" alt="Mensajes Contador incrementandose en terminal serial">
  <p><em>Salida esperada en la terminal serial</em></p>
</div>

## Errores Comunes y Variantes

| Síntoma | Causa típica |
|---|---|
| No se observa ningún mensaje en la terminal | `pico_enable_stdio_usb` no está habilitado con el valor `1` en el CMakeLists |
| Los primeros mensajes no aparecen o se ven cortados al conectar | El sistema operativo tarda un instante en enumerar el puerto USB-CDC; conviene abrir la terminal después de que el dispositivo esté disponible |
| Error de compilación relacionado con `printf` | Falta el include de `<stdio.h>` |

**Variantes:**

- Sustituir el contador simple por el tiempo transcurrido desde el arranque, obtenido con `time_us_64()`.
- Leer un caracter desde la terminal con `getchar_timeout_us(0)` e imprimir de vuelta su valor recibido (sin implementar aún un eco completo, que se aborda en la práctica de UART).
- Cambiar el intervalo de impresión y observar el efecto sobre la cantidad de líneas mostradas en la terminal.
