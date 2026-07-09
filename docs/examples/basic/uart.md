# Ejemplo 02: Comunicación UART

Corresponde a la práctica **Comunicación UART** del Módulo II. A diferencia de los ejemplos anteriores, aquí no se usa la consola USB (`stdio`) — se habla directamente con el periférico `hardware_uart`, que es el mecanismo real para conectar el RP2040 con otros dispositivos serie (GPS, módulos Bluetooth clásicos, otro microcontrolador, etc.), no solo para depurar.

## Conexiones

Requiere un adaptador USB-serial externo (no se usa el puerto USB nativo de la placa).

| Señal Pico | Pin | Conectar a |
|---|---|---|
| TX | GPIO 0 | RX del adaptador USB-serial |
| RX | GPIO 1 | TX del adaptador USB-serial |
| GND | — | GND del adaptador |

> Cruce obligatorio: el TX de un lado siempre va al RX del otro.

## Estructura del proyecto

```
02_comunicacion_uart/
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── main.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(comunicacion_uart C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(comunicacion_uart
    main.c
)

target_link_libraries(comunicacion_uart
    pico_stdlib
    hardware_uart
)

pico_add_extra_outputs(comunicacion_uart)
```

No se habilita `pico_enable_stdio_usb` ni `_uart`: el ejemplo no usa `printf`, solo llamadas directas a `hardware_uart`.

## main.c

```c
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID     uart0
#define BAUD_RATE   115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int main() {
    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_puts(UART_ID, "Eco UART listo. Escribe algo:\r\n");

    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            uart_putc(UART_ID, c);  // eco del caracter recibido

            if (c == '\r') {
                uart_puts(UART_ID, "\n");
            }
        }
    }
}
```

## Explicación

`uart_init` configura el baud rate sobre la instancia de hardware `uart0`; `gpio_set_function` es el paso que se olvida con más frecuencia — sin él, los pines siguen configurados como GPIO genérico y la UART no tiene por dónde salir aunque el periférico ya esté inicializado. El resto es polling simple: `uart_is_readable` evita bloquear el programa mientras no llega nada. El detalle de cada función está en la [guía de C, sección UART](../c-programming.md#uart-hardware_uart).

## Compilar y cargar

```bash
mkdir build && cd build
cmake ..
make
```

Ver [configuración del entorno](../devlab.md#cargar-el-programa) para el flasheo.

## Verificación

Con `minicom` o `screen` (ver [configuración del entorno](../devlab.md#minicom-o-screen)) conéctate al puerto del adaptador USB-serial —no al de la placa— a 115200 8N1. Al resetear la Pico debe aparecer el mensaje inicial, y cada carácter que escribas debe regresar en eco.

## Variantes

- Reemplazar el eco simple por un parser de comandos (`"LED ON"` / `"LED OFF"`) que controle el GPIO del [ejemplo 00](./00-parpadeo-led.md).
- Mover la comunicación a `uart1` en otro par de pines y correr ambos ejemplos simultáneamente.
- Agregar un timeout: si no llega nada en N segundos, enviar un "heartbeat".

## Errores comunes

| Síntoma | Causa típica |
|---|---|
| No se recibe ni se transmite nada | TX/RX cruzados al revés, o `gpio_set_function` omitido |
| Se ven caracteres corruptos | Baud rate distinto entre la terminal y `BAUD_RATE` en el código |
| Compila pero falla el *linking* | Falta `hardware_uart` en `target_link_libraries` |
