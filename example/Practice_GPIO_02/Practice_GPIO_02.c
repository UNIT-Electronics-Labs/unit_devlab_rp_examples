/**
 * @file Practice_GPIO_02.c
 * @brief Proyecto Practice_GPIO_02 para Raspberry Pi Pico
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
#define BUTTON_PIN 24

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    gpio_pull_up(BUTTON_PIN); // Pull up the button to avoid floating state the Chinese Board doesn't have pull up resistor in the circuit

    bool estado_anterior = gpio_get(BUTTON_PIN);

    
    if (estado_anterior) {
        printf("Button is not pressed\n");
    } else {
        printf("Button is pressed\n");
    }

    while (1) {

        bool estado_actual = gpio_get(BUTTON_PIN);

        if (estado_actual != estado_anterior) {
            
            if (estado_actual) {
                printf("Button is not pressed\n");
            } else {
                printf("Button is pressed\n");
            }

            estado_anterior = estado_actual;
        }
        sleep_ms(20); 
    }
    return 0;
}
