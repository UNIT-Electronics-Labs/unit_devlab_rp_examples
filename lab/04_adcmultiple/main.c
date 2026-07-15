#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

/* ─── Defines ──────────────────────────────────────────── */
#define NUM_CANALES 4
const uint8_t pines[NUM_CANALES]   = {26, 27, 28, 29};
const uint8_t canales[NUM_CANALES] = {0, 1, 2, 3};   // AINSEL correspondiente a cada GPIO

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();

    adc_init();

    // Inicializacion fisica de cada pin como entrada analogica:
    // deshabilita pull-ups/pull-downs y el buffer de entrada digital.
    for (int i = 0; i < NUM_CANALES; i++) {
        adc_gpio_init(pines[i]);
    }

    // Round-robin deshabilitado: el barrido lo controla el programa,
    // no el hardware del ADC.
    // adc_set_round_robin(0x0F);  // no se usa en esta variante

    while (1) {
        uint16_t muestras[NUM_CANALES];

        // Seleccion manual: por cada canal, se apunta el mux (AINSEL)
        // y se dispara una conversion individual con adc_read().
        for (int i = 0; i < NUM_CANALES; i++) {
            adc_select_input(canales[i]);
            muestras[i] = adc_read();
        }

        printf("GPIO26=%4u  GPIO27=%4u  GPIO28=%4u  GPIO29=%4u\n",
               muestras[0], muestras[1], muestras[2], muestras[3]);

        sleep_ms(1000);
    }
    return 0;
}