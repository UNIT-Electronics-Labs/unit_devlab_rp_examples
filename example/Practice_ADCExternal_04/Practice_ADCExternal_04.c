/**
 * @file Practice_ADCExternal_04.c
 * @brief Proyecto Practice_ADCExternal_04 para Raspberry Pi Pico
 *
 * @author obviousfancy
 * @date 2026-07-10
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

/* ─── Defines ──────────────────────────────────────────── */
 #define TMP235_PIN 26

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(TMP235_PIN);                      // GP26 = ADC0

    while (1) {
        uint16_t raw = adc_read();
        float voltage = raw * 3.3f / 4096.0f;
        float temp =(voltage - 0.5f) * 100;

        printf("Temperatura: %.2f C\n", temp);
        sleep_ms(1000);

    }
}