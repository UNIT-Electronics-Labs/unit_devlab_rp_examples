/**
 * @file Practice_ADC_03.c
 * @brief Proyecto Practice_ADC_03 para Raspberry Pi Pico
 *
 * @author obviousfancy
 * @date 2026-07-10
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"


int main() {
    stdio_init_all();

    adc_init();                                     //Inicializacion del ADC
    adc_set_temp_sensor_enabled(true);              // Habilitar el sensor de temperatura interno
    adc_select_input(4);                            // canal 4 = sensor de temperatura interno

    while (true) {
        uint16_t raw = adc_read();
        float voltage = raw * 3.3f / 4096.0f;
        float temp = 27.0f - (voltage - 0.706f) / 0.001721f;

        printf("Temperatura: %.2f C\n", temp);
        sleep_ms(1000);
    }
}