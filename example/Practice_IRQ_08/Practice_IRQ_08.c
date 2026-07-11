/**
 * @file Practice_IRQ_08.c
 * @brief Proyecto Practice_IRQ_08 para Raspberry Pi Pico
 *
 * @author obviousfancy
 * @date 2026-07-10
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/irq.h"

/* ─── Defines ──────────────────────────────────────────── */
// #define MY_PIN 0

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();
 // irq_set_exclusive_handler(IRQ_NUM, handler);
    // irq_set_enabled(IRQ_NUM, true);

    while (1) {
        // TODO: lógica principal del proyecto

    }
}
