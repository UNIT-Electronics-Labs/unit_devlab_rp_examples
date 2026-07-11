```C
/**
 * @file Practice_UART_05.c
 * @brief Proyecto Practice_UART_05 para Raspberry Pi Pico
 *
 * @author obviousfancy
 * @date 2026-07-10
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include "pico/stdlib.h"
#include "hardware/uart.h"

/* ─── Defines ──────────────────────────────────────────── */
#define UART_TX0 0
#define UART_RX0 1
#define UART_BAUD_RATE 115200
#define UART_ID uart0

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    //stdio_init_all();
    uart_init(UART_ID, UART_BAUD_RATE);

    gpio_set_function(UART_TX0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX0, GPIO_FUNC_UART);


    while (1) {
        // TODO: lógica principal del proyecto
        uart_puts(UART_ID, "Hello, UART!\n\r");
        sleep_ms(1000);
    }
}
```







#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID     uart0
#define BAUD_RATE   115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int main() {
    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_puts(UART_ID, "Eco UART listo. Escribe algo:\r\n");

    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            uart_puts(UART_ID, "Recibido: ");
            uart_putc(UART_ID, c);
            uart_puts(UART_ID, "\r\n");
        }
    }
}