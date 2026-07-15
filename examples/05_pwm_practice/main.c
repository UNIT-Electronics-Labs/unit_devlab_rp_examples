/**
 * @file main.c
 * @brief Atenuacion progresiva del LED integrado mediante PWM
 *
 * @author obviousfancy
 * @date 2026-07-12
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/pwm.h"

/* ─── Defines ──────────────────────────────────────────── */
#define LED_PIN 10
#define PWM_WRAP 255
/* ─── Main ─────────────────────────────────────────────── */
int main() {
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);

    // Configuración del slice y el canal específico para el LED

    uint slice   = pwm_gpio_to_slice_num(LED_PIN);              
    uint channel = pwm_gpio_to_channel(LED_PIN);

    // Configuracion completa del slice, aunque esta practica solo
    // varie el nivel de un canal de forma gradual:
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 100.0f);                  // Divisor de reloj 100 admite 8 bits 0-255
    pwm_config_set_wrap(&config, PWM_WRAP);                        // Periodo del contador: 256 pasos (0-255) admie 16 bits 65535
    pwm_config_set_phase_correct(&config, false);             // Conteo simple (no simetrico) De 0 a wrap - wrap a 0 falso
    pwm_config_set_output_polarity(&config, false, false);    // Polaridad normal en ambos canales A y B

    pwm_init(slice, &config, true);  // true = arrancar de inmediato , si es falso activar con pwm_set_enabled(slice, true)

    pwm_set_chan_level(slice, channel, 0); // inicializar el canal a 0


    int nivel = 0;
    int paso = 5;


    while (1) {
        // TODO: lógica principal del proyecto
        pwm_set_chan_level(slice, channel, (uint16_t)nivel);

        nivel += paso;
        if (nivel >= PWM_WRAP) { nivel = PWM_WRAP; paso = -paso; }
        else if (nivel <= 0) { nivel = 0; paso = -paso; }

        sleep_ms(15);
    }
    return 0;
}

