# Visualización del TMP235 en Pantalla OLED (Pico SDK)

Extiende la práctica **Lectura analógica del TMP235 mediante ADC (Pico SDK)** agregando una pantalla OLED SSD1306 por I²C para mostrar los valores en tiempo real. A diferencia del driver `ssd1306.c` usado en la práctica de I²C del Módulo II —que deliberadamente solo dibuja un rectángulo, sin texto—, este ejemplo requiere mostrar números en pantalla, por lo que el driver se extiende con una fuente bitmap propia y funciones de dibujo de texto.

## Objetivo

Leer el sensor TMP235 mediante el ADC y mostrar en una pantalla OLED SSD1306 (128×64, I²C) el valor crudo del ADC, el voltaje calculado y la temperatura resultante, usando texto renderizado directamente sobre el framebuffer.

## Hardware

| Componente | Cantidad |
|---|---|
| Raspberry Pi Pico (RP2040) | 1 |
| Sensor TMP235 (empaque SOT-23) | 1 |
| Pantalla OLED SSD1306 128×64 (I²C) | 1 |
| Cables de conexión | 7 |

## Conexiones

**TMP235** (ver también [Lectura analógica del TMP235 mediante ADC](./adcTMP235.md)):

| Pin del TMP235 | Señal | Conexión en la Pico |
|---|---|---|
| 1 | VDD | 3V3(OUT) |
| 2 | VOUT | GPIO26 (ADC0) |
| 3 | GND | GND |

**Pantalla OLED SSD1306** (mismo bus I2C0 usado en la práctica de I²C del Módulo II):

| Señal | Pin | Notas |
|---|---|---|
| SDA | GPIO4 | Con pull-up (interno o el que ya trae el módulo OLED) |
| SCL | GPIO5 | Con pull-up |
| VCC | 3.3V | |
| GND | GND | |

## Estructura del proyecto

```
tmp235_oled_sdk/
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

project(tmp235_oled_sdk C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(tmp235_oled_sdk
    main.c
    ssd1306.c
)

target_link_libraries(tmp235_oled_sdk
    pico_stdlib
    hardware_adc
    hardware_i2c
)

pico_add_extra_outputs(tmp235_oled_sdk)
pico_enable_stdio_usb(tmp235_oled_sdk 1)
pico_enable_stdio_uart(tmp235_oled_sdk 0)
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
void ssd1306_set_pixel(uint8_t x, uint8_t y, bool encendido);
void ssd1306_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool encendido);
void ssd1306_draw_hline(uint8_t x, uint8_t y, uint8_t w, bool encendido);
void ssd1306_draw_char(uint8_t x, uint8_t y, char c);
void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str);
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
    for (unsigned i = 0; i < sizeof(secuencia_init); i++) {
        write_cmd(secuencia_init[i]);
    }
}

void ssd1306_clear(void) {
    memset(buffer, 0x00, sizeof(buffer));
}

void ssd1306_set_pixel(uint8_t x, uint8_t y, bool encendido) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    uint16_t idx = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bit = y % 8;
    if (encendido) {
        buffer[idx] |= (1u << bit);
    } else {
        buffer[idx] &= ~(1u << bit);
    }
}

void ssd1306_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool encendido) {
    for (uint8_t j = 0; j < h; j++) {
        for (uint8_t i = 0; i < w; i++) {
            ssd1306_set_pixel(x + i, y + j, encendido);
        }
    }
}

void ssd1306_draw_hline(uint8_t x, uint8_t y, uint8_t w, bool encendido) {
    for (uint8_t i = 0; i < w; i++) {
        ssd1306_set_pixel(x + i, y, encendido);
    }
}

// Fuente 5x7: cada caracter son 5 columnas de 8 bits (bit0 = fila superior).
// Se usan solo los 7 bits inferiores; el bit mas alto queda en 0 como
// separacion vertical entre renglones de texto. Cubre unicamente el
// conjunto de caracteres usado por este ejemplo (mayusculas, digitos y
// unos pocos simbolos), no el alfabeto ASCII completo.
typedef struct {
    char c;
    uint8_t col[5];
} glyph_t;

static const glyph_t FONT[] = {
    {' ', {0x00,0x00,0x00,0x00,0x00}},
    {'.', {0x00,0x60,0x60,0x00,0x00}},
    {'-', {0x08,0x08,0x08,0x08,0x08}},
    {'=', {0x14,0x14,0x14,0x14,0x14}},
    {'0', {0x3E,0x51,0x49,0x45,0x3E}},
    {'1', {0x00,0x42,0x7F,0x40,0x00}},
    {'2', {0x42,0x61,0x51,0x49,0x46}},
    {'3', {0x21,0x41,0x45,0x4B,0x31}},
    {'4', {0x18,0x14,0x12,0x7F,0x10}},
    {'5', {0x27,0x45,0x45,0x45,0x39}},
    {'6', {0x3C,0x4A,0x49,0x49,0x30}},
    {'7', {0x01,0x71,0x09,0x05,0x03}},
    {'8', {0x36,0x49,0x49,0x49,0x36}},
    {'9', {0x06,0x49,0x49,0x29,0x1E}},
    {'A', {0x7E,0x11,0x11,0x11,0x7E}},
    {'C', {0x3E,0x41,0x41,0x41,0x22}},
    {'M', {0x7F,0x02,0x0C,0x02,0x7F}},
    {'P', {0x7F,0x09,0x09,0x09,0x06}},
    {'R', {0x7F,0x09,0x19,0x29,0x46}},
    {'T', {0x01,0x01,0x7F,0x01,0x01}},
    {'V', {0x1F,0x20,0x40,0x20,0x1F}},
    {'W', {0x3F,0x40,0x38,0x40,0x3F}},
};

#define FONT_COUNT (sizeof(FONT) / sizeof(FONT[0]))
#define GLYPH_WIDTH  5
#define GLYPH_ADVANCE 6  // 5 columnas + 1 de espacio entre caracteres

static const glyph_t *buscar_glyph(char c) {
    for (unsigned i = 0; i < FONT_COUNT; i++) {
        if (FONT[i].c == c) return &FONT[i];
    }
    return &FONT[0];  // caracter no encontrado: se dibuja espacio en blanco
}

void ssd1306_draw_char(uint8_t x, uint8_t y, char c) {
    const glyph_t *g = buscar_glyph(c);
    for (uint8_t col = 0; col < GLYPH_WIDTH; col++) {
        uint8_t bits = g->col[col];
        for (uint8_t row = 0; row < 7; row++) {
            bool encendido = (bits >> row) & 0x01;
            ssd1306_set_pixel(x + col, y + row, encendido);
        }
    }
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str) {
    uint8_t cursor_x = x;
    while (*str) {
        ssd1306_draw_char(cursor_x, y, *str);
        cursor_x += GLYPH_ADVANCE;
        str++;
    }
}

void ssd1306_show(void) {
    // Ventana de direccionamiento: pantalla completa (128 columnas, 8 paginas)
    write_cmd(0x21); write_cmd(0x00); write_cmd(SSD1306_WIDTH - 1);
    write_cmd(0x22); write_cmd(0x00); write_cmd(0x07);

    uint8_t data[SSD1306_WIDTH + 1];
    data[0] = 0x40;  // 0x40 = "lo que sigue son datos de GDDRAM"
    for (int page = 0; page < SSD1306_HEIGHT / 8; page++) {
        memcpy(&data[1], &buffer[page * SSD1306_WIDTH], SSD1306_WIDTH);
        i2c_write_blocking(i2c0, SSD1306_I2C_ADDR, data, SSD1306_WIDTH + 1, false);
    }
}
```

## main.c

```c
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include <stdio.h>

#define I2C_PORT           i2c0
#define I2C_SDA_PIN        4
#define I2C_SCL_PIN        5
#define TMP235_ADC_PIN     26
#define TMP235_ADC_CHANNEL 0

int main(void) {
    stdio_init_all();

    // ADC: lectura del TMP235
    adc_init();
    adc_gpio_init(TMP235_ADC_PIN);
    adc_select_input(TMP235_ADC_CHANNEL);

    // I2C: pantalla OLED SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_init();

    char linea1[22];
    char linea2[22];
    char linea3[22];

    while (true) {
        uint16_t raw = adc_read();                      // 0 a 4095
        float voltage_mV = raw * 3300.0f / 4096.0f;      // 3.3 V = 3300 mV
        float temp_c = (voltage_mV - 500.0f) / 10.0f;    // Funcion de transferencia

        snprintf(linea1, sizeof(linea1), "RAW=%u", raw);
        snprintf(linea2, sizeof(linea2), "V=%.1fMV", voltage_mV);
        snprintf(linea3, sizeof(linea3), "T=%.2fC", temp_c);

        ssd1306_clear();
        ssd1306_draw_string(0, 0, "TMP235");
        ssd1306_draw_hline(0, 16, SSD1306_WIDTH, true);
        ssd1306_draw_string(0, 24, linea1);
        ssd1306_draw_string(0, 36, linea2);
        ssd1306_draw_string(0, 48, linea3);
        ssd1306_show();

        printf("raw=%u  V=%.1f mV  T=%.2f C\n", raw, voltage_mV, temp_c);

        sleep_ms(300);
    }
}
```

## Explicación

**Por qué el driver anterior no alcanza:** el `ssd1306.c` de la práctica de I²C dibuja únicamente un rectángulo que se desplaza por la pantalla — suficiente para confirmar que la comunicación I²C funciona, pero sin capacidad de mostrar texto o números. Esta práctica necesita mostrar valores numéricos reales, así que el driver se extiende con:

- `ssd1306_set_pixel()`: primitiva de bajo nivel que traduce una coordenada (x, y) al byte y bit correspondientes del framebuffer.
- Una fuente bitmap propia de 5×7 (`FONT[]`), donde cada carácter se representa como 5 columnas de 8 bits (7 usados, 1 de separación vertical). Cubre únicamente el conjunto de caracteres que usa esta práctica (mayúsculas, dígitos, `.`, `-`, `=`, espacio), no el alfabeto ASCII completo.
- `ssd1306_draw_char()` / `ssd1306_draw_string()`: recorren los bits de cada columna del glifo y llaman a `ssd1306_set_pixel()`.

**Formato de los datos:** `snprintf()` arma cada línea de texto en un buffer (`linea1`, `linea2`, `linea3`) antes de dibujarla, igual que se haría con `sprintf` en cualquier programa en C — no hay nada específico del RP2040 en esto, solo formateo de cadenas.

**Uso del ADC e I²C:** idénticos a las prácticas ya validadas — `adc_gpio_init(26)` + `adc_select_input(0)` para la lectura analógica, y `gpio_set_function(..., GPIO_FUNC_I2C)` + `i2c_init()` para el bus, siguiendo el mismo criterio de wiring que la práctica de I²C del Módulo II (I2C0 en GP4/GP5).

## Verificación

Al iniciar, la pantalla debe mostrar el título "TMP235", una línea horizontal separadora, y tres líneas con `RAW=`, `V=` y `T=` actualizándose cada 300 ms. Por consola (USB) se imprime la misma información para contrastar valores.

## Errores comunes

| Síntoma | Causa típica |
|---|---|
| Pantalla en blanco, sin errores de compilación | Dirección I2C incorrecta (verificar con un escaneo de bus si el módulo no usa `0x3C`) |
| Caracteres no reconocidos aparecen en blanco | El carácter no está en la tabla `FONT[]`; `buscar_glyph()` devuelve un espacio en blanco por defecto |
| Texto cortado o superpuesto | Cadena más larga que el ancho disponible (128 px ÷ 6 px por carácter ≈ 21 caracteres por línea) |
| Falla en el *linking* | Falta `hardware_i2c` o `hardware_adc` en `target_link_libraries` |

## Variantes

- Ampliar la tabla `FONT[]` con minúsculas o signos adicionales si se necesitan en otro layout.
- Aumentar el tamaño de los caracteres duplicando cada píxel (escala 2×), replicando el efecto de `setTextSize(2)` de la versión en Arduino.
- Sustituir los valores numéricos por una representación gráfica simple (barra o tendencia), reutilizando `ssd1306_fill_rect()`.
