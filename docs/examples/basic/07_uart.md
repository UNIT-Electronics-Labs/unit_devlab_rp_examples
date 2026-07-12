# Ejemplo 02: Comunicación UART

Una vez cubiertos el control de un GPIO como salida ([ejemplo 00](./00_blink.md)) y como entrada digital ([ejemplo 01](./02_gpio.md)), la presente práctica introduce la comunicación serial asíncrona (UART), el primero de los protocolos de comunicación abordados en el Módulo II. Se presentan dos enfoques complementarios: el puerto serial virtual expuesto sobre USB, ya empleado de manera implícita en los ejemplos anteriores para mostrar mensajes de depuración, y el periférico UART de hardware dedicado, que permite establecer comunicación con un dispositivo externo independiente de la computadora host. Contrastar ambos enfoques resulta relevante porque, si bien comparten buena parte de la interfaz de programación (`printf`, `getchar`, etc.), corresponden a mecanismos de transporte físicamente distintos.

## Objetivo general
 
Configurar la comunicación serial asíncrona (UART) del RP2040 para la transmisión y recepción de datos, contrastando el uso del puerto serial virtual expuesto por USB (`stdio_usb`) frente al periférico UART de hardware dedicado (UART0), y estableciendo el formato de trama y la velocidad de transmisión requeridos para una comunicación confiable.

## Primera parte: UART sobre USB (stdio_usb)
 
### Hardware requerido (USB)
 
| Componente | Cantidad | Observaciones |
|---|---|---|
| Placa con RP2040 (Raspberry Pi Pico o equivalente) | 1 | Plataforma empleada en el Módulo II |
 
### Conexiones (USB)
 
No se requiere ninguna conexión adicional. La comunicación se realiza a través del mismo cable USB empleado para la programación y consola de la placa, mediante el puerto serial virtual (`stdio_usb`) que expone el SDK.
 
### Estructura del proyecto (USB)
 
```
Practice_UART_USB_01/
├── build
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── Practice_UART_USB_01.c
```
 
### CMakeLists.txt (USB)
 
```cmake
cmake_minimum_required(VERSION 3.13)
 
include(pico_sdk_import.cmake)
 
# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por pico-new
# ─────────────────────────────────────────────
set(PROJECT_NAME    "Practice_UART_USB_01")
set(PROJECT_SOURCES "Practice_UART_USB_01.c")
set(PICO_BOARD      "pico")
 
set(PROJECT_LIBS
    pico_stdlib
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
 
### Código fuente (USB) — `Practice_UART_USB_01.c`
 
```c
#include <stdio.h>
#include "pico/stdlib.h"
 
int main() {
    stdio_init_all();
 
    printf("=== Eco por UART sobre USB ===\n");
    printf("Escriba un caracter:\n");
 
    while (true) {
        int c = getchar_timeout_us(0);  // Lectura no bloqueante
 
        if (c != PICO_ERROR_TIMEOUT) {
            printf("Recibido: '%c' (0x%02X)\n", c, c);
            putchar(c);  // Retransmite el mismo caracter (eco)
        }
    }
}
```
 
### Explicación (USB)
 
`stdio_init_all()` habilita, entre otras interfaces, el puerto serial virtual sobre USB cuando `pico_enable_stdio_usb` está activo en el CMakeLists; a partir de ese punto, `printf`, `putchar` y `getchar_timeout_us` operan sobre dicho canal de la misma manera en que lo harían sobre una UART física. `getchar_timeout_us(0)` realiza una lectura no bloqueante: retorna de inmediato con el valor `PICO_ERROR_TIMEOUT` cuando no hay datos disponibles, en lugar de detener la ejecución del programa a la espera de un caracter. Este enfoque resulta conveniente para prototipos rápidos, aunque no sustituye a la UART de hardware cuando se requiere comunicación con un dispositivo externo distinto de la computadora host.
 
### Errores comunes (USB)
 
| Síntoma | Causa típica |
|---|---|
| No se observa ningún mensaje en la terminal | `pico_enable_stdio_usb` no está habilitado con el valor `1` en el CMakeLists |
| Los caracteres escritos no se reflejan de vuelta | La terminal utilizada no está mostrando el dato retransmitido por `putchar(c)`; verificar la configuración de eco local de la terminal |
| El programa parece detenerse esperando entrada | Se sustituyó `getchar_timeout_us(0)` por `getchar()`, que sí es bloqueante |
 
---

## Segunda parte: UART de hardware (UART0)

### Hardware requerido (hardware)
 
| Componente | Cantidad | Observaciones |
|---|---|---|
| Placa con RP2040 (Raspberry Pi Pico o equivalente) | 1 | Plataforma empleada en el Módulo II |
| UNIT DevLab MultiHub Shield | 1 | Plataforma empleada en el Modulo II |
| Adaptador USB-serial (FTDI, CP2102, CH340 o equivalente) | 1 | Incluido en el UNIT DevLab MultiHub Shield |
| Cables de conexión (jumper) | 3 (o 1 en loopback) | Para las señales TX, RX y GND |

| Señal | Pin del RP2040 | Descripción |
|---|---|---|
| TX (UART0) | GP0 | Salida de datos; se conecta al pin RX del adaptador USB-serial externo |
| RX (UART0) | GP1 | Entrada de datos; se conecta al pin TX del adaptador USB-serial externo |
| GND | GND | Referencia de tierra común entre la placa y el adaptador |
 
> **Nota:** en ausencia de un adaptador USB-serial externo, puede realizarse una prueba de lazo cerrado (loopback) conectando directamente GP0 con GP1 mediante un jumper; en ese caso, el propio RP2040 recibe los datos que transmite.

### Estructura del proyecto (hardware)
 
```
Practice_UART_05/
├── build
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── Practice_UART_05.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por pico-new
# ─────────────────────────────────────────────
set(PROJECT_NAME    "Practice_UART_05")
set(PROJECT_SOURCES "Practice_UART_05.c")
set(PICO_BOARD      "pico")

set(PROJECT_LIBS
    pico_stdlib
    hardware_uart
)
# ─────────────────────────────────────────────
# NO MODIFICAR DE AQUÍ EN ADELANTE
# ─────────────────────────────────────────────

project(${PROJECT_NAME})

pico_sdk_init()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})

pico_add_extra_outputs(${PROJECT_NAME})


```

No se habilita `pico_enable_stdio_usb` ni `_uart`: el ejemplo no usa `printf`, solo llamadas directas a `hardware_uart`.

### Código fuente (hardware) — `Practice_UART_05.c`

```c
/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/uart.h"

/* ─── Defines ──────────────────────────────────────────── */
#define UART_TX0 0
#define UART_RX0 1
#define UART_BAUD_RATE 115200
#define UART_ID uart0
#define LED_PIN 25

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    //stdio_init_all();
    uart_init(UART_ID, UART_BAUD_RATE);

    // Configurar la función de los pines GPIO para UART
    gpio_set_function(UART_TX0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX0, GPIO_FUNC_UART);
    // Configurar LED integrado para feedback visual
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Mensaje de bienvenida inicial
    uart_puts(UART_ID, "\r\n--- UART Eco e Interacción Iniciada ---\r\n");
    uart_puts(UART_ID, "Envía 'E' para encender el LED, 'A' para apagarlo.\r\n");


    while (1) {
    if (uart_is_readable(UART_ID)) {
            // Leer el caracter entrante (Bloqueante solo si no se validara antes)
            char ch = uart_getc(UART_ID);

            // 1. Efecto Eco: Devolver el caracter recibido al usuario
            uart_puts(UART_ID, "Recibido: ");
            uart_putc(UART_ID, ch);
            uart_puts(UART_ID, "\r\n");

            // 2. Lógica de control básica
            if (ch == 'E' || ch == 'e') {
                gpio_put(LED_PIN, 1);
                uart_puts(UART_ID, ">> LED ENCENDIDO\r\n");
            } 
            else if (ch == 'A' || ch == 'a') {
                gpio_put(LED_PIN, 0);
                uart_puts(UART_ID, ">> LED APAGADO\r\n");
            }
        }
    }
}

```

### Explicación (hardware)
 
A diferencia del puerto serial virtual sobre USB, la UART de hardware requiere configurar explícitamente tres aspectos: la velocidad de transmisión mediante `uart_init`, la función alternativa de los pines GP0/GP1 mediante `gpio_set_function(..., GPIO_FUNC_UART)` —sin esta llamada, los pines permanecen en su función GPIO por defecto y no quedan conectados al periférico UART— y el formato de trama mediante `uart_set_format`, en este caso 8N1 (8 bits de datos, sin paridad, 1 bit de parada), que debe coincidir con la configuración del dispositivo externo. La FIFO de hardware, habilitada mediante `uart_set_fifo_enabled`, amortigua ráfagas cortas de datos sin requerir que el programa atienda cada byte de manera inmediata; no obstante, es necesario consultarla con suficiente frecuencia para evitar su desbordamiento, razón por la cual el ciclo principal se ejecuta cada 10 ms. El reporte por `printf` se mantiene sobre USB, de modo que ambos canales —UART0 y el puerto virtual— pueden observarse de forma simultánea y por separado.
 
### Errores comunes (hardware)
 
| Síntoma | Causa típica |
|---|---|
| No se transmite ni recibe nada por UART0 | Falta `gpio_set_function()` sobre GP0/GP1, o los pines quedaron configurados con la función incorrecta |
| Se reciben caracteres corruptos o ilegibles | Velocidad (baudrate) o formato de trama distintos entre el RP2040 y el dispositivo externo |
| Se pierden bytes durante ráfagas de datos | FIFO no habilitada, o el ciclo principal consulta `uart_is_readable()` con demasiada poca frecuencia |
| Error de *linking* durante la compilación | Ausencia de `hardware_uart` en `target_link_libraries` |
 
---
 
## Compilación y carga
 
Ambas partes se compilan y cargan de la misma manera, mediante el depurador SWD (CH552), tal como se describe en el [ejemplo 00](./00_blink.md):
 
```bash
pico-flash
```
 
Consúltese la [configuración del entorno](../../guide/devlab.md#cargar-el-programa) para el detalle del procedimiento.
 
## Verificación
 
Ábrase una terminal serial sobre el puerto USB-CDC que expone la placa (por ejemplo, `/dev/ttyACM0` en Linux) a 115200 baudios:
 
```bash
minicom -b 115200 -D /dev/ttyACM0
```
 
En la primera parte, cada caracter escrito en la terminal debe reflejarse de vuelta (eco), acompañado de su valor hexadecimal. En la segunda parte, deben observarse por USB los reportes de cada byte recibido por UART0; si se emplea un adaptador USB-serial externo, dicho adaptador puede monitorearse desde una segunda terminal en el host para confirmar la recepción de los datos transmitidos por el RP2040.
 
<div align="center">
  <img src="../resources/uart5.png" width="300px" alt="Comunicación UART mostrada en terminal serial">
  <p><em>Salida esperada en la terminal serial</em></p>
</div>
## Variantes
 
- Implementar un shell interactivo simple que reconozca comandos de texto (por ejemplo, `help`, `led on`, `led off`, `status`) y ejecute la acción correspondiente sobre el LED del [ejemplo 00](./00_blink.md).
- Sustituir el sondeo (`uart_is_readable`) por una interrupción de recepción (`irq_set_exclusive_handler` sobre `UART0_IRQ`), almacenando los datos en un buffer circular para procesarlos fuera de la rutina de interrupción.
- Diseñar un parser de comandos con argumentos (por ejemplo, `led 1 on`).
- Definir un protocolo binario simple con verificación mediante checksum.
- Retransmitir datos entre UART0 y UART1 a manera de puente (*bridge*) entre dos dispositivos seriales.
 