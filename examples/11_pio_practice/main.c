/**
 * @file main.c
 * @brief Matriz de 5x5 neopixels (WS2812) controlada por una maquina de
 *        estados PIO: una onda de color recorre la matriz sin intervencion
 *        del procesador salvo por el envio de cada cuadro.
 *
 * @author obviousfancy
 * @date 2026-07-14
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

/* ─── Defines ──────────────────────────────────────────── */
#define DATA_PIN     6      // GPIO conectado al DIN de la matriz
#define MATRIX_ROWS  5
#define MATRIX_COLS  5
#define NUM_PIXELS   (MATRIX_ROWS * MATRIX_COLS)
#define WS2812_FREQ  800000.f
#define BRIGHTNESS   40      // 0-255, para no exceder la corriente de la fuente

/* ─── Prototipos ───────────────────────────────────────── */
static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
static uint32_t wheel(uint8_t pos);
static uint matrix_index(uint row, uint col);

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, DATA_PIN, WS2812_FREQ);

    uint8_t frame = 0;
    while (1) {
        // Cada pixel toma un color segun su distancia en diagonal al origen
        // mas el desplazamiento del cuadro actual, generando una onda que
        // recorre la matriz de esquina a esquina.
        for (uint row = 0; row < MATRIX_ROWS; row++) {
            for (uint col = 0; col < MATRIX_COLS; col++) {
                uint8_t pos = (uint8_t)((row + col) * 32 + frame);
                uint32_t color = wheel(pos);
                put_pixel(pio, sm, color);
            }
        }
        frame += 4;
        sleep_ms(30);
    }
    return 0;
}

/* ─── Funciones ────────────────────────────────────────── */

// Envia un pixel (formato GRB de 24 bits) a la FIFO de la maquina de estados.
// El corrimiento a la izquierda de 8 bits alinea los 24 bits utiles con el
// extremo mas significativo del registro, tal como lo espera ws2812.pio.
static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

// Combina componentes R, G, B (0-255) en el orden GRB que exige el WS2812,
// aplicando BRIGHTNESS como escala global.
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    r = (uint8_t)((uint32_t)r * BRIGHTNESS / 255);
    g = (uint8_t)((uint32_t)g * BRIGHTNESS / 255);
    b = (uint8_t)((uint32_t)b * BRIGHTNESS / 255);
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | (uint32_t)b;
}

// Genera un color continuo a lo largo de una rueda cromatica de 256 pasos.
static uint32_t wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        return urgb_u32(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos -= 85;
        return urgb_u32(0, pos * 3, 255 - pos * 3);
    } else {
        pos -= 170;
        return urgb_u32(pos * 3, 255 - pos * 3, 0);
    }
}

// Traduce coordenadas (fila, columna) al indice fisico del pixel. Se asume
// cableado en serpentina (boustrophedon), el mas comun en modulos de 5x5 ya
// ensamblados: las filas pares avanzan de izquierda a derecha y las impares
// en sentido inverso. Ajustar aqui si la matriz propia usa otro cableado.
static uint matrix_index(uint row, uint col) {
    if (row % 2 == 0) {
        return row * MATRIX_COLS + col;
    } else {
        return row * MATRIX_COLS + (MATRIX_COLS - 1 - col);
    }
}
