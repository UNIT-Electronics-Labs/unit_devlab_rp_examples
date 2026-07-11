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
#define LED_PIN 25

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    //stdio_init_all();
    uart_init(UART_ID, UART_BAUD_RATE);

    // Configurar la función de los pines GPIO para UART
    gpio_set_function(UART_TX0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX0, GPIO_FUNC_UART);
    // Configurar LED integrado para feedback visual
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Mensaje de bienvenida inicial
    uart_puts(UART_ID, "\r\n--- UART Eco e Interacción Iniciada ---\r\n");
    uart_puts(UART_ID, "Envía 'E' para encender el LED, 'A' para apagarlo.\r\n");


    while (1) {
    if (uart_is_readable(UART_ID)) {
            // Leer el caracter entrante (Bloqueante solo si no se validara antes)
            char ch = uart_getc(UART_ID);

            // 1. Efecto Eco: Devolver el caracter recibido al usuario
            uart_puts(UART_ID, "Recibido: ");
            uart_putc(UART_ID, ch);
            uart_puts(UART_ID, "\r\n");

            // 2. Lógica de control básica
            if (ch == 'E' || ch == 'e') {
                gpio_put(LED_PIN, 1);
                uart_puts(UART_ID, ">> LED ENCENDIDO\r\n");
            } 
            else if (ch == 'A' || ch == 'a') {
                gpio_put(LED_PIN, 0);
                uart_puts(UART_ID, ">> LED APAGADO\r\n");
            }
        }
    }
}
