/**
 * @file main.c
 * @brief Core0 lee el sensor TMP235 por ADC y envia la temperatura por FIFO;
 *        Core1 se dedica solo a refrescar el OLED, para que el paralelismo
 *        real entre nucleos sea visible: la temperatura se actualiza al
 *        ritmo de Core0, mientras un contador propio de Core1 sigue
 *        avanzando de forma independiente, a su propio ritmo.
 *
 * @author obviousfancy
 * @date 2026-07-14
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

/* ─── Defines ──────────────────────────────────────────── */
#define TMP235_PIN 28   // GPIO28 = canal ADC 2

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

/* ─── Tarea de Core1: solo dibuja en el OLED ──────────────
 * El FIFO de hardware solo transporta enteros de 32 bits, asi que la
 * temperatura se envia multiplicada por 100 (dos decimales de precision)
 * y aqui se vuelve a dividir para reconstruir el valor en punto flotante.
 */
void core1_main() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_t display;
    display.external_vcc = false;
    ssd1306_init(&display, OLED_WIDTH, OLED_HEIGHT, OLED_ADDR, I2C_PORT);
    ssd1306_clear(&display);

    float temperatura = 0.0f;
    uint32_t contador = 0;
    uint32_t core_origen = 0;   // nucleo que realmente envio el dato, leido de su registro CPUID
    char titulo[]  = "MULTICORE";
    char linea_temp[16];
    char linea_c0[16];
    char linea_c1[16];

    // Ancho de caracter de la fuente integrada: 5 px de glifo + 1 px de
    // separacion, multiplicado por la escala usada al dibujar el texto.
    const uint32_t ANCHO_CHAR = 6;

    while (1) {
        // Lectura no bloqueante: si Core0 aun no ha enviado un dato nuevo,
        // Core1 no se detiene a esperarlo y sigue con su propio ciclo.
        if (multicore_fifo_rvalid()) {
            // El emisor viaja empacado junto con la temperatura en la
            // misma palabra de 32 bits (16 bits altos = numero de nucleo,
            // 16 bits bajos = temperatura*100). Ese numero de nucleo lo
            // calcula get_core_num() del lado de Core0 leyendo el
            // registro CPUID de hardware, no esta escrito a mano aqui.
            uint32_t dato = multicore_fifo_pop_blocking();
            core_origen = dato >> 16;
            int16_t centesimas = (int16_t)(dato & 0xFFFF);
            temperatura = centesimas / 100.0f;
        }

        // Este contador avanza solo con el ritmo de Core1 (cada 50 ms),
        // sin depender en absoluto de cuando llegue el siguiente dato del
        // ADC: es la evidencia visual de que ambos nucleos corren en
        // paralelo, cada uno con su propio reloj de ejecucion.
        contador++;

        // get_core_num() se llama aqui mismo, dentro de la tarea que
        // corre en Core1, asi que el "1" que se imprime tambien viene
        // directo del registro CPUID y no de un literal fijo.
        snprintf(linea_temp, sizeof(linea_temp), "%.1fC", temperatura);
        snprintf(linea_c0, sizeof(linea_c0), "Core%lu:ADC", core_origen);
        snprintf(linea_c1, sizeof(linea_c1), "Core%u:%lu", get_core_num(), contador);

        ssd1306_clear(&display);

        // --- Encabezado: titulo "en negrita" (se dibuja dos veces con
        // un pixel de desplazamiento) para que resalte sobre el resto ---
        uint32_t x_titulo = (OLED_WIDTH - (uint32_t)strlen(titulo) * ANCHO_CHAR) / 2;
        ssd1306_draw_string(&display, x_titulo,     0, 1, titulo);
        ssd1306_draw_string(&display, x_titulo + 1, 0, 1, titulo);
        ssd1306_draw_line(&display, 0, 10, OLED_WIDTH - 1, 10);

        // --- Cuerpo: temperatura en texto grande, centrada ---
        uint32_t x_temp = (OLED_WIDTH - (uint32_t)strlen(linea_temp) * ANCHO_CHAR * 3) / 2;
        ssd1306_draw_string(&display, x_temp, 16, 3, linea_temp);
        ssd1306_draw_line(&display, 0, 44, OLED_WIDTH - 1, 44);

        // --- Pie: Core0 a la izquierda, Core1 a la derecha ---
        ssd1306_draw_string(&display, 0, 52, 1, linea_c0);
        uint32_t x_c1 = OLED_WIDTH - (uint32_t)strlen(linea_c1) * ANCHO_CHAR;
        ssd1306_draw_string(&display, x_c1, 52, 1, linea_c1);

        ssd1306_show(&display);

        sleep_ms(50);
    }
}

/* ─── Main (Core0): solo lee el ADC y lo envia por FIFO ─── */
int main() {
    stdio_init_all();

    adc_init();
    adc_set_clkdiv(0);
    adc_gpio_init(TMP235_PIN);
    adc_select_input(2);   // Canal 2 corresponde a GPIO28

    multicore_launch_core1(core1_main);

    while (1) {
        uint16_t muestra = adc_read();
        float voltaje = muestra * 3.3f / 4096.0f;
        float temperatura = (voltaje - 0.5f) * 100.0f;   // Formula TMP235

        // Se envia como entero (temperatura * 100) porque la FIFO de
        // hardware solo mueve valores de 32 bits, no floats. El numero de
        // nucleo se obtiene con get_core_num(), que lee el registro CPUID
        // del propio Core0 en tiempo de ejecucion (no es un valor fijo), y
        // se empaca en los 16 bits altos de la misma palabra.
        uint32_t nucleo_actual = get_core_num();
        int16_t centesimas = (int16_t)(temperatura * 100.0f);
        uint32_t dato = (nucleo_actual << 16) | ((uint16_t)centesimas);
        multicore_fifo_push_blocking(dato);

        printf("Core%lu: Temperatura enviada = %.2f C\n", nucleo_actual, temperatura);
        sleep_ms(1000);
    }
    return 0;
}
