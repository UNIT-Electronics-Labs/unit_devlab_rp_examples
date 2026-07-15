/**
 * @file main.c
 * @brief Control independiente de dos LEDs mediante temporizadores repetitivos
 *        de hardware, usando user_data para reutilizar un unico callback
 *
 * @author obviousfancy
 * @date 2026-07-15
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

/* ─── Defines ──────────────────────────────────────────── */
#define LED_PIN_1      10
#define LED_PIN_2      15
#define INTERVALO_1_MS 500
#define INTERVALO_2_MS 300

/* ─── Contexto por LED ──────────────────────────────────────
 * Cada instancia de esta estructura representa el estado
 * propio de un LED: su pin fisico, su estado logico actual
 * y un contador de parpadeos. Se pasa como user_data para
 * que un unico callback pueda manejar multiples LEDs sin
 * variables globales ni funciones duplicadas.
 */
typedef struct {
    uint pin;
    bool estado;
    uint32_t contador;
} led_context_t;

static led_context_t led1_ctx = { .pin = LED_PIN_1, .estado = false, .contador = 0 };
static led_context_t led2_ctx = { .pin = LED_PIN_2, .estado = false, .contador = 0 };

/* ─── Callback del temporizador ────────────────────────────
 * Se invoca automaticamente cada vez que su timer asociado
 * cumple el intervalo configurado. El parametro "t" da acceso
 * a t->user_data, que aqui apunta al led_context_t especifico
 * de cada timer, permitiendo reusar la misma funcion para
 * ambos LEDs sin mezclar su estado.
 */
bool repetir_callback(struct repeating_timer *t) {
    led_context_t *ctx = (led_context_t *)t->user_data;

    ctx->estado = !ctx->estado;
    ctx->contador++;
    gpio_put(ctx->pin, ctx->estado);

    printf("[LED pin=%2u] estado=%s | parpadeos=%lu\n",
           ctx->pin,
           ctx->estado ? "ON " : "OFF",
           (unsigned long)ctx->contador);

    return true;
}

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();

    gpio_init(LED_PIN_1);
    gpio_set_dir(LED_PIN_1, GPIO_OUT);

    gpio_init(LED_PIN_2);
    gpio_set_dir(LED_PIN_2, GPIO_OUT);

    struct repeating_timer timer1;
    struct repeating_timer timer2;

    // Signo negativo: intervalo fijo desde el inicio de cada
    // disparo, evita drift acumulado por el tiempo de ejecucion
    // del propio callback.
    add_repeating_timer_ms(-INTERVALO_1_MS, repetir_callback, &led1_ctx, &timer1);
    add_repeating_timer_ms(-INTERVALO_2_MS, repetir_callback, &led2_ctx, &timer2);

    while (1) {
        tight_loop_contents();
    }
    return 0;
}