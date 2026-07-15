/**
 * @file main.c
 * @brief Proyecto 00_blink_practice para Raspberry Pi Pico
 *
 * @author obviousfancy
 * @date 2026-07-12
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* ─── Defines ──────────────────────────────────────────── */
#define LED_PIN 25
#define LEDEXT_PIN 8

/* Lista de pines de salida: agregar aquí cualquier nuevo pin */
static const uint OUTPUT_PINS[] = {
    LED_PIN,
    LEDEXT_PIN,
};
#define OUTPUT_PINS_COUNT (sizeof(OUTPUT_PINS) / sizeof(OUTPUT_PINS[0]))

void initPins() {
    for (size_t i = 0; i < OUTPUT_PINS_COUNT; i++) {
        gpio_init(OUTPUT_PINS[i]);
        gpio_set_dir(OUTPUT_PINS[i], GPIO_OUT);
    }
}
/* ─── Main ─────────────────────────────────────────────── */
int main() {
    initPins();

    while (1) {
        for (size_t i = 0; i < OUTPUT_PINS_COUNT; i++) {
            gpio_put(OUTPUT_PINS[i], 1);   // Enciende todos los pines
        }
        sleep_ms(1000);                    // Retardo de 1 segundo

        for (size_t i = 0; i < OUTPUT_PINS_COUNT; i++) {
            gpio_put(OUTPUT_PINS[i], 0);   // Apaga todos los pines
        }
        sleep_ms(1000);                    // Retardo de 1 segundo
    }
}
