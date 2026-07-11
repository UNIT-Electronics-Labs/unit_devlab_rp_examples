/**
 * @file Practice_I2C_06.c
 * @brief Proyecto Practice_I2C_06 para Raspberry Pi Pico
 *
 * @author obviousfancy
 * @date 2026-07-10
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/i2c.h"

/* ─── Defines ──────────────────────────────────────────── */
#define I2C_SDA 0
#define I2C_SCL 1
#define I2C_PORT i2c0
#define I2C_FREQ 400000

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    while (1) {
        // TODO: lógica principal del proyecto

    }
}
