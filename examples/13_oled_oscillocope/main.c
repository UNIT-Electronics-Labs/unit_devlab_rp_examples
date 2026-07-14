/**
 * @file main.c
 * @brief Osciloscopio OLED de un canal, 500 kS/s
 *
 * Entrada analogica: GP26 / ADC0 (solo de 0 V a 3.3 V).
 * Pantalla SSD1306: SDA GP0, SCL GP1, direccion 0x3C.
 */

#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "ssd1306.h"

/* Pantalla OLED SSD1306 128x64. */
#define I2C_PORT       i2c0
#define I2C_SDA_PIN    0
#define I2C_SCL_PIN    1
#define OLED_ADDRESS   0x3C
#define OLED_WIDTH     128
#define OLED_HEIGHT    64

/* 500 kS/s es la velocidad maxima nominal del ADC del RP2040. */
#define ADC_GPIO_PIN   26
#define ADC_INPUT      0
#define SAMPLE_RATE_HZ 500000u
#define CAPTURE_SAMPLES 1024u
#define SAMPLES_PER_PIXEL 4u
#define SCREEN_SAMPLES (OLED_WIDTH * SAMPLES_PER_PIXEL)

#define GRAPH_TOP      11
#define GRAPH_BOTTOM   63
#define TRIGGER_X      16u
#define PRETRIGGER_SAMPLES (TRIGGER_X * SAMPLES_PER_PIXEL)
#define MIN_SIGNAL_SPAN 32u

static uint16_t samples[CAPTURE_SAMPLES];
static uint16_t display_minimum;
static uint16_t display_maximum;
static bool display_scale_ready;

/** Captura un bloque sin que la actualizacion I2C altere el tiempo de muestreo. */
static void capture_block(uint dma_channel, const dma_channel_config *dma_config) {
    adc_run(false);
    adc_fifo_drain();

    dma_channel_configure(
        dma_channel,
        dma_config,
        samples,
        &adc_hw->fifo,
        CAPTURE_SAMPLES,
        true
    );

    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);
    adc_run(false);
}

/** Busca un flanco ascendente con histeresis para rechazar ruido. */
static uint32_t find_trigger(uint16_t minimum, uint16_t maximum) {
    const uint16_t span = maximum - minimum;
    if (span < MIN_SIGNAL_SPAN) {
        return 0;
    }

    const uint16_t low_level = minimum + (uint16_t)((uint32_t)span * 45u / 100u);
    const uint16_t high_level = minimum + (uint16_t)((uint32_t)span * 55u / 100u);
    const uint32_t last_trigger =
        CAPTURE_SAMPLES - SCREEN_SAMPLES + PRETRIGGER_SAMPLES;
    bool armed = samples[PRETRIGGER_SAMPLES] <= low_level;

    for (uint32_t i = PRETRIGGER_SAMPLES + 1u; i < last_trigger; ++i) {
        if (samples[i] <= low_level) {
            armed = true;
        } else if (armed && samples[i] >= high_level) {
            return i - PRETRIGGER_SAMPLES;
        }
    }

    return 0;
}

static int32_t sample_to_y(uint16_t sample, uint16_t minimum, uint16_t maximum) {
    const uint32_t graph_height = GRAPH_BOTTOM - GRAPH_TOP;

    if (maximum == minimum) {
        return GRAPH_TOP + graph_height / 2;
    }

    if (sample <= minimum) return GRAPH_BOTTOM;
    if (sample >= maximum) return GRAPH_TOP;

    return GRAPH_BOTTOM - (int32_t)(((uint32_t)(sample - minimum) * graph_height)
                                    / (maximum - minimum));
}

/** Expande la escala de inmediato y la contrae lentamente entre capturas. */
static void update_display_scale(uint16_t minimum, uint16_t maximum) {
    const uint16_t span = maximum - minimum;
    const uint16_t margin = span / 16u + 4u;
    const uint16_t target_minimum = minimum > margin ? minimum - margin : 0u;
    const uint16_t target_maximum = maximum < (4095u - margin)
        ? maximum + margin
        : 4095u;

    if (!display_scale_ready) {
        display_minimum = target_minimum;
        display_maximum = target_maximum;
        display_scale_ready = true;
        return;
    }

    if (target_minimum < display_minimum) {
        display_minimum = target_minimum;
    } else {
        display_minimum = (uint16_t)((7u * display_minimum + target_minimum) / 8u);
    }

    if (target_maximum > display_maximum) {
        display_maximum = target_maximum;
    } else {
        display_maximum = (uint16_t)((7u * display_maximum + target_maximum) / 8u);
    }
}

/** Promedia varias conversiones para representar cada columna de la OLED. */
static uint16_t sample_for_pixel(uint32_t first, uint32_t x) {
    const uint32_t start = first + x * SAMPLES_PER_PIXEL;
    uint32_t sum = 0;

    for (uint32_t i = 0; i < SAMPLES_PER_PIXEL; ++i) {
        sum += samples[start + i];
    }

    return (uint16_t)(sum / SAMPLES_PER_PIXEL);
}

static void draw_capture(ssd1306_t *display) {
    uint16_t minimum = 4095;
    uint16_t maximum = 0;

    for (uint32_t i = 0; i < CAPTURE_SAMPLES; ++i) {
        if (samples[i] < minimum) minimum = samples[i];
        if (samples[i] > maximum) maximum = samples[i];
    }

    const uint32_t first = find_trigger(minimum, maximum);
    update_display_scale(minimum, maximum);
    char status[24];
    snprintf(status, sizeof(status), "500kS  1.02ms");

    ssd1306_clear(display);
    ssd1306_draw_string(display, 0, 0, 1, status);

    /* Reticula: nivel medio y divisiones de 0.256 ms. */
    for (int32_t x = 0; x < OLED_WIDTH; x += 32) {
        for (int32_t y = GRAPH_TOP; y <= GRAPH_BOTTOM; y += 4) {
            ssd1306_draw_pixel(display, x, y);
        }
    }
    for (int32_t x = 0; x < OLED_WIDTH; x += 4) {
        ssd1306_draw_pixel(display, x, (GRAPH_TOP + GRAPH_BOTTOM) / 2);
    }

    int32_t previous_y = sample_to_y(
        sample_for_pixel(first, 0), display_minimum, display_maximum);
    for (uint32_t x = 1; x < OLED_WIDTH; ++x) {
        const int32_t y = sample_to_y(
            sample_for_pixel(first, x), display_minimum, display_maximum);
        ssd1306_draw_line(display, x - 1, previous_y, x, y);
        previous_y = y;
    }

    ssd1306_show(display);
}

int main(void) {
    stdio_init_all();

    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_t display = {.external_vcc = false};
    if (!ssd1306_init(
            &display, OLED_WIDTH, OLED_HEIGHT, OLED_ADDRESS, I2C_PORT)) {
        printf("No se pudo inicializar la pantalla OLED.\n");
        while (true) tight_loop_contents();
    }

    adc_init();
    adc_gpio_init(ADC_GPIO_PIN);
    adc_select_input(ADC_INPUT);
    adc_fifo_setup(true, true, 1, false, false);

    /* El ADC recibe 48 MHz. Cada muestra empieza cada (1 + divisor) ciclos. */
    const float adc_divider =
        (float)clock_get_hz(clk_adc) / (float)SAMPLE_RATE_HZ - 1.0f;
    adc_set_clkdiv(adc_divider);

    const uint dma_channel = dma_claim_unused_channel(true);
    dma_channel_config dma_config = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_config, false);
    channel_config_set_write_increment(&dma_config, true);
    channel_config_set_dreq(&dma_config, DREQ_ADC);

    while (true) {
        capture_block(dma_channel, &dma_config);
        draw_capture(&display);
    }
}
