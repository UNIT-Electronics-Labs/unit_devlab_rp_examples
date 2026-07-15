#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

/* ─── Defines ──────────────────────────────────────────── */
#define NUM_CANALES 4
const uint8_t pines[NUM_CANALES] = {26, 27, 28, 29};

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();

    adc_init();

    for (int i = 0; i < NUM_CANALES; i++) {
        adc_gpio_init(pines[i]);
    }

    // AINSEL inicial: el primer canal que el round-robin va a muestrear.
    // No es obligatorio que coincida con un bit activo en RROBIN, pero
    // aqui se deja en el canal 0 para que el barrido inicie en GPIO26.
    adc_select_input(0);

    // RROBIN es una mascara de 5 bits (uno por canal, incluyendo el
    // sensor interno = bit 4). 0x0F = 0b01111 activa los canales 0-3
    // (GPIO26-29) y excluye el canal 4 (sensor interno).
    adc_set_round_robin(0x0F);

    while (1) {
        uint16_t muestras[NUM_CANALES];

        // Cada adc_read() dispara una conversion individual (START_ONCE);
        // el round-robin solo decide automaticamente CUAL canal se lee
        // en cada disparo, avanzando AINSEL tras cada conversion.
        for (int i = 0; i < NUM_CANALES; i++) {
            muestras[i] = adc_read();
        }

        printf("GPIO26=%4u  GPIO27=%4u  GPIO28=%4u  GPIO29=%4u\n",
               muestras[0], muestras[1], muestras[2], muestras[3]);

        sleep_ms(1000);
    }
    return 0;
}