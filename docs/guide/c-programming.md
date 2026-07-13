# Programación en C para RP2040

Esta guía funciona como referencia rápida de sintaxis para la programación del RP2040 en C mediante el Pico SDK. No sustituye la explicación conceptual de cada periférico —disponible en su práctica correspondiente dentro de "Periféricos Básicos"—, sino que reúne, en un solo lugar, la forma correcta de invocar las funciones más empleadas a lo largo del curso.

## Estructura Básica de un Proyecto

Todo proyecto basado en el Pico SDK comparte la misma anatomía mínima: uno o más archivos fuente en C, un `CMakeLists.txt` que orquesta la compilación, y el archivo `pico_sdk_import.cmake` que localiza el SDK en el sistema. El directorio `build/`, generado por CMake durante la compilación, no debe versionarse ni compartirse entre equipos, ya que su contenido depende de la instalación local.

```
mi_proyecto/
├── main.c                      # Código fuente principal
├── CMakeLists.txt              # Configuración de compilación
├── pico_sdk_import.cmake       # Localizador del SDK
└── build/                      # Generado por CMake; no se versiona
    ├── main.elf
    ├── main.uf2
    ├── main.bin
    ├── main.hex
    └── CMakeCache.txt
```

### CMakeLists.txt

La herramienta `pico-new` genera automáticamente el `CMakeLists.txt` de cada proyecto siguiendo una plantilla fija, empleada de manera consistente a lo largo de todo el curso:

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por pico-new
# ─────────────────────────────────────────────
set(PROJECT_NAME    "Mi_Proyecto")
set(PROJECT_SOURCES "main.c")
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

Las únicas secciones que habitualmente requieren modificarse son `PROJECT_NAME`, `PROJECT_SOURCES` (cuando el proyecto incluye más de un archivo fuente) y `PROJECT_LIBS` (agregando la biblioteca `hardware_*` correspondiente a cada periférico empleado).

### main.c básico

```c
/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"

/* ─── Defines ──────────────────────────────────────────── */
#define LED_PIN 25

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}
```

## Librería Estándar del Pico SDK

A continuación se resume la sintaxis de inicialización y uso básico de los periféricos cubiertos en el curso. El detalle conceptual de cada uno —incluyendo diagramas y cálculos, cuando corresponde— se documenta en su práctica respectiva.

### GPIO (pico_stdlib)

```c
#include "pico/stdlib.h"

// Inicializar pin
gpio_init(pin);

// Configurar dirección
gpio_set_dir(pin, GPIO_OUT);  // Salida
gpio_set_dir(pin, GPIO_IN);   // Entrada

// Escribir
gpio_put(pin, 1);  // Nivel alto
gpio_put(pin, 0);  // Nivel bajo

// Leer
bool estado = gpio_get(pin);

// Resistencias internas (mutuamente excluyentes entre si)
gpio_pull_up(pin);
gpio_pull_down(pin);
```

### UART (hardware_uart)

```c
#include "hardware/uart.h"

// Inicializar UART0 a 115200 baudios
uart_init(uart0, 115200);

// Configurar pines
gpio_set_function(0, GPIO_FUNC_UART);  // TX
gpio_set_function(1, GPIO_FUNC_UART);  // RX

// Transmitir
uart_puts(uart0, "Hola\n");
uart_putc(uart0, 'A');

// Recibir (consulta no bloqueante)
if (uart_is_readable(uart0)) {
    char c = uart_getc(uart0);
}
```

### I2C (hardware_i2c)

```c
#include "hardware/i2c.h"

// Inicializar I2C0 a 400 kHz
i2c_init(i2c0, 400000);

// Configurar pines
gpio_set_function(0, GPIO_FUNC_I2C);  // SDA
gpio_set_function(1, GPIO_FUNC_I2C);  // SCL
gpio_pull_up(0);
gpio_pull_up(1);

// Escribir a un dispositivo (ejemplo: comando a un display SSD1306)
uint8_t data[] = {0x00, 0x3F};
i2c_write_blocking(i2c0, 0x3C, data, 2, false);

// Leer de un dispositivo
uint8_t buffer[10];
i2c_read_blocking(i2c0, 0x3C, buffer, 10, false);
```

### SPI (hardware_spi)

```c
#include "hardware/spi.h"

// Inicializar SPI0 a 1 MHz
spi_init(spi0, 1000000);

// MISO, SCK y MOSI se asignan a la función SPI
gpio_set_function(16, GPIO_FUNC_SPI);  // MISO
gpio_set_function(18, GPIO_FUNC_SPI);  // SCK
gpio_set_function(19, GPIO_FUNC_SPI);  // MOSI

// El pin CS se maneja como GPIO comun: el SDK del RP2040
// no ofrece control automatico de Chip Select
gpio_init(17);
gpio_set_dir(17, GPIO_OUT);
gpio_put(17, 1);  // Inactivo en reposo (activo en bajo)

// Transmitir datos (CS debe bajarse manualmente antes de la transacción)
gpio_put(17, 0);
uint8_t tx_data[] = {0x01, 0x02, 0x03};
spi_write_blocking(spi0, tx_data, 3);
gpio_put(17, 1);
```

### ADC (hardware_adc)

```c
#include "hardware/adc.h"

// Inicializar ADC
adc_init();

// Habilitar sensor de temperatura interno
adc_set_temp_sensor_enabled(true);
adc_select_input(4);  // Canal 4 = sensor interno de temperatura

// Leer valor
uint16_t raw = adc_read();
float voltage = raw * 3.3f / 4096.0f;

// Convertir a temperatura (aproximacion lineal del fabricante)
float temp = 27.0f - (voltage - 0.706f) / 0.001721f;
```

### Temporizadores (pico_time)

```c
#include "pico/time.h"

// Retardos bloqueantes
sleep_ms(1000);    // 1 segundo
sleep_us(100);     // 100 microsegundos

// Marca de tiempo actual, en microsegundos desde el arranque
uint64_t ahora = time_us_64();

// Temporizador repetitivo
bool timer_callback(struct repeating_timer *t) {
    // Codigo a ejecutar en cada intervalo
    return true;  // true = continuar repitiendo
}

struct repeating_timer timer;
add_repeating_timer_ms(-500, timer_callback, NULL, &timer);
```

> **Nota sobre el signo del intervalo:** un valor negativo programa cada repetición a un periodo fijo, medido desde el **inicio** de la invocación anterior, independientemente de cuánto tarde el propio callback; un valor positivo la mide desde el **final** de la invocación anterior, por lo que un callback lento alarga el periodo real. Se recomienda el valor negativo salvo que se requiera explícitamente el otro comportamiento.

## Interrupciones GPIO

```c
#include "hardware/gpio.h"

void gpio_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_RISE) {
        // Flanco ascendente
    }
    if (events & GPIO_IRQ_EDGE_FALL) {
        // Flanco descendente
    }
}

int main() {
    gpio_init(2);
    gpio_set_dir(2, GPIO_IN);
    gpio_pull_up(2);

    gpio_set_irq_enabled_with_callback(2,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &gpio_callback);

    while (1) tight_loop_contents();
}
```

> **Importante:** `gpio_set_irq_enabled_with_callback()` registra la retrollamada compartida para todas las interrupciones GPIO de un núcleo. Si se necesita habilitar una interrupción sobre un segundo pin, la llamada subsecuente debe emplear `gpio_set_irq_enabled()` en su lugar; repetir `gpio_set_irq_enabled_with_callback()` sobrescribiría la retrollamada ya registrada.

## Compilación y Flasheo

### Compilar

```bash
cmake -S . -B build
cmake --build build
```

### Cargar en la placa

La programación de la placa se realiza exclusivamente mediante el depurador SWD (CH552 CMSIS-DAP), a través del script `pico-flash`:

```bash
pico-flash
```

Este script invoca a OpenOCD internamente y transfiere el binario directamente a la memoria flash del microcontrolador. El sitio no emplea el modo de almacenamiento masivo (BOOTSEL) ni la copia manual de archivos `.uf2` en ningún momento del curso.

### Mensajes de depuración por USB

```c
#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();

    while (1) {
        printf("Temperatura: %.2f C\n", temp);
        sleep_ms(1000);
    }
}
```

### Proyecto multi-archivo

Cuando la lógica de un proyecto crece más allá de lo razonable para un único archivo, conviene separarla en pares de archivos `.h`/`.c`, listando todos los fuentes en `PROJECT_SOURCES`:

```cmake
set(PROJECT_SOURCES main.c sensor.c)
```

```c
// sensor.h
#ifndef SENSOR_H
#define SENSOR_H

float leer_temperatura(void);

#endif
```

```c
// sensor.c
#include "sensor.h"
#include "hardware/adc.h"

float leer_temperatura(void) {
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096.0f;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}
```

### Combinación de varias bibliotecas

Un mismo proyecto puede requerir más de una biblioteca `hardware_*` de manera simultánea, cuando combina varios periféricos —tal como ocurre en la mayoría de las aplicaciones de la sección "Aplicaciones e Integración":

```cmake
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_i2c
    hardware_adc
)
```

Cada biblioteca adicional debe listarse explícitamente; ninguna se enlaza de manera implícita por el solo hecho de incluir su encabezado (`#include`) en el código fuente.

## Mejores Prácticas

1. Inicializar siempre cada periférico antes de usarlo, respetando el orden: inicialización del periférico, asignación de pines, configuración de parámetros.
2. Emplear constantes con nombre para números de pin (`#define LED_PIN 25`), en lugar de literales numéricos dispersos en el código.
3. Verificar los valores de retorno relevantes —el baudrate o frecuencia real reportado por las funciones `_init()`, o el resultado de una lectura I2C/SPI— en lugar de asumir que toda operación tiene éxito.
4. Documentar el código con el encabezado oficial (`@author`, `@board`, `@sdk`) y con comentarios que expliquen el motivo de una decisión, no únicamente qué instrucción se ejecuta.
5. Modularizar en archivos `.h`/`.c` separados una vez que el proyecto supere un único archivo razonable.
6. Dejar comentada, con una breve explicación, cualquier configuración disponible en el periférico pero no necesaria para el caso de uso actual, en lugar de omitirla en silencio o activarla sin necesidad real.
7. Evitar operaciones bloqueantes prolongadas dentro de una rutina de interrupción.
8. Cargar siempre el programa mediante `pico-flash` (SWD); nunca mediante el modo BOOTSEL ni la copia manual de archivos `.uf2`.

## Recursos

- [Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [C SDK Examples](https://github.com/raspberrypi/pico-examples)
