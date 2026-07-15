/**
 * @file main.c
 * @brief Lectura del sensor de temperatura interno mediante el ADC
 *
 * @author obviousfancy
 * @date 2026-07-12
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

/* ─── Defines ──────────────────────────────────────────── */
// #define MY_PIN 0
#define TMP235_PIN 28   // GPIO28 = canal ADC 2

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();

    adc_init();

    adc_gpio_init(TMP235_PIN);
    adc_select_input(2);                  // Canal 2 corresponde a GPIO28

 
    while (1) {
        uint16_t muestra = adc_read();
        float voltaje = muestra * 3.3f / 4096.0f;
        // Formula del sensor interno, ya no aplica con el TMP235 externo:
        // float temperatura = 27.0f - (voltaje - 0.706f) / 0.001721f;
        float temperatura = (voltaje - 0.5f) * 100.0f;   // Formula TMP235

        printf("Temperatura: %.2f C\n", temperatura);
        sleep_ms(1000);
    }
    return 0;
}

