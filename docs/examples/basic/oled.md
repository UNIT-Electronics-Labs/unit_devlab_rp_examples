# Ejemplo 04: Comunicación I²C con display OLED

Corresponde a la práctica **Comunicación I²C con display OLED** del Módulo II. Es el ejemplo multi-archivo de esta sección: separa el driver del display (`ssd1306.h` / `ssd1306.c`) del programa principal, siguiendo el mismo patrón de proyecto modular que ya se documenta en la [guía de C](../../guide/c-programming.md#proyecto-multi-archivo).

No implementa texto ni fuentes — eso complica el driver sin aportar nada nuevo sobre I²C. En su lugar dibuja un cuadro que rebota de un lado a otro, suficiente para confirmar que la comunicación funciona y para entender cómo se arma un framebuffer.

## Conexiones

| Señal | Pin | Notas |
|---|---|---|
| SDA | GPIO 4 | Con pull-up (interno o el que ya trae el módulo OLED) |
| SCL | GPIO 5 | Con pull-up |
| VCC | 3.3V | La mayoría de los módulos SSD1306 aceptan 3.3V; verificar el módulo específico |
| GND | GND | |

## Estructura del proyecto

```
04_i2c_oled/
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── main.c
├── ssd1306.h
└── ssd1306.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(i2c_oled C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(i2c_oled
    main.c
    ssd1306.c
)

target_link_libraries(i2c_oled
    pico_stdlib
    hardware_i2c
)

pico_add_extra_outputs(i2c_oled)
```

## ssd1306.h

```c
#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>

#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64

void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool encendido);
void ssd1306_show(void);

#endif
```

## ssd1306.c

```c
#include "ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>

// Framebuffer: 1 bit por pixel, organizado en 8 "paginas" de 128 bytes.
static uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static void write_cmd(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};  // 0x00 = "lo que sigue es un comando"
    i2c_write_blocking(i2c0, SSD1306_I2C_ADDR, data, 2, false);
}

void ssd1306_init(void) {
    static const uint8_t secuencia_init[] = {
        0xAE,       // display off
        0x20, 0x00, // modo de direccionamiento: horizontal
        0xB0,       // direccion de pagina inicial
        0xC8,       // direccion de escaneo COM: invertida
        0x00, 0x10, // columna baja / alta
        0x40,       // linea de inicio = 0
        0x81, 0x7F, // contraste
        0xA1,       // remapeo de segmento
        0xA6,       // display normal (no invertido)
        0xA8, 0x3F, // relacion de multiplexado = 64
        0xA4,       // muestra contenido de RAM
        0xD3, 0x00, // offset de display = 0
        0xD5, 0x80, // divisor de reloj / frecuencia de oscilador
        0xD9, 0xF1, // periodo de precarga
        0xDA, 0x12, // configuracion de pines COM
        0xDB, 0x40, // nivel VCOMH
        0x8D, 0x14, // charge pump: habilitado
        0xAF        // display on
    };

    for (size_t i = 0; i < sizeof(secuencia_init); i++) {
        write_cmd(secuencia_init[i]);
    }

    ssd1306_clear();
    ssd1306_show();
}

void ssd1306_clear(void) {
    memset(buffer, 0x00, sizeof(buffer));
}

void ssd1306_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool encendido) {
    for (uint8_t j = y; j < y + h && j < SSD1306_HEIGHT; j++) {
        for (uint8_t i = x; i < x + w && i < SSD1306_WIDTH; i++) {
            uint16_t index = i + (j / 8) * SSD1306_WIDTH;
            uint8_t  bit   = j % 8;
            if (encendido) {
                buffer[index] |= (1 << bit);
            } else {
                buffer[index] &= ~(1 << bit);
            }
        }
    }
}

void ssd1306_show(void) {
    write_cmd(0x21); write_cmd(0); write_cmd(SSD1306_WIDTH - 1);  // rango de columnas
    write_cmd(0x22); write_cmd(0); write_cmd(7);                   // rango de paginas (8)

    uint8_t data[SSD1306_WIDTH + 1];
    data[0] = 0x40;  // 0x40 = "lo que sigue son datos de pixeles"

    for (int page = 0; page < 8; page++) {
        memcpy(&data[1], &buffer[page * SSD1306_WIDTH], SSD1306_WIDTH);
        i2c_write_blocking(i2c0, SSD1306_I2C_ADDR, data, SSD1306_WIDTH + 1, false);
    }
}
```

## main.c

```c
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_SDA 4
#define I2C_SCL 5

int main() {
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    uint8_t x = 0;
    bool creciendo = true;

    while (true) {
        ssd1306_clear();
        ssd1306_fill_rect(x, 24, 16, 16, true);  // cuadro de 16x16 en movimiento
        ssd1306_show();

        if (creciendo) {
            x += 4;
            if (x >= SSD1306_WIDTH - 16) creciendo = false;
        } else {
            x -= 4;
            if (x == 0) creciendo = true;
        }

        sleep_ms(80);
    }
}
```

## Explicación

El SSD1306 organiza su memoria en 8 "páginas" verticales de 8 pixeles cada una; cada byte que se envía controla una columna completa de 8 pixeles en una página. Por eso el framebuffer es un arreglo plano de 1024 bytes (128 columnas × 8 páginas) y `ssd1306_fill_rect` hace la traducción de coordenadas (x, y) a esa organización con `/ 8` y `% 8`.

La razón para usar un framebuffer en vez de escribir pixel por pixel es de rendimiento: cada `i2c_write_blocking` tiene overhead de transacción I²C, así que agrupar todos los cambios en memoria y enviarlos de una sola vez en `ssd1306_show()` es mucho más eficiente que 1024 transacciones individuales. Este es exactamente el patrón sobre el que se construyen las librerías de fuentes/texto: una vez que existe el framebuffer, dibujar caracteres es solo prender los bits correctos.

El detalle de `i2c_init`, `i2c_write_blocking` y la configuración de pines está en la [guía de C, sección I²C](../../guide/c-programming.md#i²c-hardware_i2c).

## Compilar y cargar

```bash
mkdir build && cd build
cmake ..
make
```

Ver [configuración del entorno](../../guide/devlab.md#cargar-el-programa) para el flasheo.

## Verificación

Un cuadro de 16×16 pixeles rebota horizontalmente en la pantalla. Si la pantalla queda en blanco pero no hay errores de compilación, lo primero a revisar es la dirección I²C (ver tabla de errores).

## Variantes

- Agregar una fuente 5×7 y una función `ssd1306_draw_text` para mostrar strings.
- Controlar la velocidad del rebote con el botón del [ejemplo 01](./02_gpio.md).
- Mostrar la temperatura del [ejemplo 03](./04_adc.md) en pantalla en vez de imprimirla por USB.
- Dibujar dos figuras independientes para practicar composición de framebuffer.

## Errores comunes

| Síntoma | Causa típica |
|---|---|
| Pantalla completamente en blanco | Dirección incorrecta: algunos módulos SSD1306 usan `0x3D` en vez de `0x3C` — probar ambas |
| Pantalla en blanco, sin importar la dirección | Falta `gpio_set_function` en SDA/SCL, o el módulo no tiene alimentación |
| Se ve "ruido" en vez del cuadro | Buffer corrupto: revisar los límites en `ssd1306_fill_rect` si se modificó |
| Falla el *linking* | Falta `hardware_i2c` en `target_link_libraries` |
