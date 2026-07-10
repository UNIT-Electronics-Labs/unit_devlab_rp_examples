/**
 * @file Practice_Blink_01.c
 * @brief Proyecto Practice_Blink_01 para Raspberry Pi Pico
 *
 * @author obviousfancy
 * @date 2026-07-10
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* ─── Defines ──────────────────────────────────────────── */
#define LED_PIN 25

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        // TODO: lógica principal del proyecto
        gpio_put(LED_PIN, 1); // Turn on LED
        printf("LED ON\n");
        sleep_ms(1000); // Delay for 1 second
        gpio_put(LED_PIN, 0); // Turn off LED
        printf("LED OFF\n");
        sleep_ms(1000); // Delay for 1 second
    }
}
