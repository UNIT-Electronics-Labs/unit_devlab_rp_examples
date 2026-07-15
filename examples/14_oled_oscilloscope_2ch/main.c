/**
 * @file main.c
 * @brief Osciloscopio OLED de dos canales, 500 kS/s por canal
 *
 * Entradas analogicas: GP26 / ADC0 y GP27 / ADC1 (solo de 0 V a 3.3 V).
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

/* El RP2040 tiene un ADC: los canales se capturan en bloques independientes. */
#define ADC0_GPIO_PIN  26
#define ADC1_GPIO_PIN  27
#define ADC0_INPUT     0
#define ADC1_INPUT     1
#define ADC_SAMPLE_RATE_HZ 500000u
#define CAPTURE_SAMPLES_PER_CHANNEL 1024u
#define SETTLING_SAMPLES 8u
#define DMA_CAPTURE_SAMPLES (CAPTURE_SAMPLES_PER_CHANNEL + SETTLING_SAMPLES)
#define SAMPLES_PER_PIXEL 4u
#define SCREEN_SAMPLES (OLED_WIDTH * SAMPLES_PER_PIXEL)

#define CH1_GRAPH_TOP  11
#define CH1_GRAPH_BOTTOM 36
#define CH2_GRAPH_TOP  38
#define CH2_GRAPH_BOTTOM 63
#define TRIGGER_X      16u
#define PRETRIGGER_SAMPLES (TRIGGER_X * SAMPLES_PER_PIXEL)
#define MIN_SIGNAL_SPAN 32u
#define MIN_DISPLAY_SPAN 128u

static uint16_t dma_samples[DMA_CAPTURE_SAMPLES];
static uint16_t channel_samples[2][CAPTURE_SAMPLES_PER_CHANNEL];

typedef struct {
    uint16_t minimum;
    uint16_t maximum;
    bool ready;
} display_scale_t;

static display_scale_t display_scale[2];

/** Selecciona y captura un canal; descarta muestras mientras se asienta el MUX. */
static void capture_channel(
    uint adc_input,
    uint16_t *destination,
    uint dma_channel,
    const dma_channel_config *dma_config
) {
    adc_run(false);
    adc_fifo_drain();
    adc_select_input(adc_input);

    dma_channel_configure(
        dma_channel,
        dma_config,
        dma_samples,
        &adc_hw->fifo,
        DMA_CAPTURE_SAMPLES,
        true
    );

    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);
    adc_run(false);

    /* Las primeras conversiones pueden conservar carga del canal anterior. */
    for (uint32_t i = 0; i < CAPTURE_SAMPLES_PER_CHANNEL; ++i) {
        destination[i] = dma_samples[SETTLING_SAMPLES + i];
    }
}

static void capture_both_channels(
    uint dma_channel,
    const dma_channel_config *dma_config
) {
    capture_channel(ADC0_INPUT, channel_samples[0], dma_channel, dma_config);
    capture_channel(ADC1_INPUT, channel_samples[1], dma_channel, dma_config);
}

/** Busca un flanco ascendente con histeresis para rechazar ruido. */
static uint32_t find_trigger(
    const uint16_t *samples,
    uint16_t minimum,
    uint16_t maximum
) {
    const uint16_t span = maximum - minimum;
    if (span < MIN_SIGNAL_SPAN) {
        return 0;
    }

    const uint16_t low_level = minimum + (uint16_t)((uint32_t)span * 45u / 100u);
    const uint16_t high_level = minimum + (uint16_t)((uint32_t)span * 55u / 100u);
    const uint32_t last_trigger =
        CAPTURE_SAMPLES_PER_CHANNEL - SCREEN_SAMPLES + PRETRIGGER_SAMPLES;
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

static int32_t sample_to_y(
    uint16_t sample,
    uint16_t minimum,
    uint16_t maximum,
    int32_t graph_top,
    int32_t graph_bottom
) {
    const uint32_t graph_height = (uint32_t)(graph_bottom - graph_top);

    if (maximum == minimum) {
        return graph_top + (int32_t)graph_height / 2;
    }

    if (sample <= minimum) return graph_bottom;
    if (sample >= maximum) return graph_top;

    return graph_bottom - (int32_t)(((uint32_t)(sample - minimum) * graph_height)
                                    / (maximum - minimum));
}

/** Expande una escala de inmediato y la contrae lentamente. */
static void approach_display_scale(
    display_scale_t *scale,
    uint16_t target_minimum,
    uint16_t target_maximum
) {
    if (!scale->ready) {
        scale->minimum = target_minimum;
        scale->maximum = target_maximum;
        scale->ready = true;
        return;
    }

    if (target_minimum < scale->minimum) {
        scale->minimum = target_minimum;
    } else {
        scale->minimum = (uint16_t)((7u * scale->minimum + target_minimum) / 8u);
    }

    if (target_maximum > scale->maximum) {
        scale->maximum = target_maximum;
    } else {
        scale->maximum = (uint16_t)((7u * scale->maximum + target_maximum) / 8u);
    }
}

/** Usa igual sensibilidad vertical, pero centra cada canal en su propio nivel DC. */
static void update_shared_display_scale(
    const uint16_t minimum[2],
    const uint16_t maximum[2]
) {
    uint16_t common_span = maximum[0] - minimum[0];
    const uint16_t channel_2_span = maximum[1] - minimum[1];

    if (channel_2_span > common_span) common_span = channel_2_span;
    if (common_span < MIN_DISPLAY_SPAN) common_span = MIN_DISPLAY_SPAN;

    const uint16_t margin = common_span / 16u + 4u;
    uint32_t display_span = (uint32_t)common_span + 2u * margin;
    if (display_span > 4095u) display_span = 4095u;

    for (uint32_t channel = 0; channel < 2u; ++channel) {
        const uint32_t center =
            ((uint32_t)minimum[channel] + maximum[channel]) / 2u;
        int32_t target_minimum = (int32_t)center - (int32_t)display_span / 2;

        if (target_minimum < 0) target_minimum = 0;
        if ((uint32_t)target_minimum + display_span > 4095u) {
            target_minimum = (int32_t)(4095u - display_span);
        }

        approach_display_scale(
            &display_scale[channel],
            (uint16_t)target_minimum,
            (uint16_t)((uint32_t)target_minimum + display_span));
    }
}

/** Promedia varias conversiones para representar cada columna de la OLED. */
static uint16_t sample_for_pixel(
    const uint16_t *samples,
    uint32_t first,
    uint32_t x
) {
    const uint32_t start = first + x * SAMPLES_PER_PIXEL;
    uint32_t sum = 0;

    for (uint32_t i = 0; i < SAMPLES_PER_PIXEL; ++i) {
        sum += samples[start + i];
    }

    return (uint16_t)(sum / SAMPLES_PER_PIXEL);
}

static void channel_limits(
    const uint16_t *samples,
    uint16_t *minimum,
    uint16_t *maximum
) {
    *minimum = 4095u;
    *maximum = 0u;

    for (uint32_t i = 0; i < CAPTURE_SAMPLES_PER_CHANNEL; ++i) {
        if (samples[i] < *minimum) *minimum = samples[i];
        if (samples[i] > *maximum) *maximum = samples[i];
    }
}

static void draw_channel(
    ssd1306_t *display,
    const uint16_t *samples,
    uint32_t first,
    const display_scale_t *scale,
    int32_t graph_top,
    int32_t graph_bottom
) {
    const int32_t graph_middle = (graph_top + graph_bottom) / 2;

    for (int32_t x = 0; x < OLED_WIDTH; x += 32) {
        for (int32_t y = graph_top; y <= graph_bottom; y += 4) {
            ssd1306_draw_pixel(display, x, y);
        }
    }
    for (int32_t x = 0; x < OLED_WIDTH; x += 4) {
        ssd1306_draw_pixel(display, x, graph_middle);
    }

    int32_t previous_y = sample_to_y(
        sample_for_pixel(samples, first, 0),
        scale->minimum,
        scale->maximum,
        graph_top,
        graph_bottom);
    for (uint32_t x = 1; x < OLED_WIDTH; ++x) {
        const int32_t y = sample_to_y(
            sample_for_pixel(samples, first, x),
            scale->minimum,
            scale->maximum,
            graph_top,
            graph_bottom);
        ssd1306_draw_line(display, (int32_t)x - 1, previous_y, (int32_t)x, y);
        previous_y = y;
    }
}

static void draw_capture(ssd1306_t *display) {
    uint16_t channel_minimum[2];
    uint16_t channel_maximum[2];

    for (uint32_t channel = 0; channel < 2u; ++channel) {
        channel_limits(
            channel_samples[channel],
            &channel_minimum[channel],
            &channel_maximum[channel]);
    }
    update_shared_display_scale(channel_minimum, channel_maximum);

    /* Cada bloque usa su propio disparo porque las capturas no son simultaneas. */
    uint32_t first[2];
    for (uint32_t channel = 0; channel < 2u; ++channel) {
        first[channel] = find_trigger(
            channel_samples[channel],
            channel_minimum[channel],
            channel_maximum[channel]);
    }
    const uint16_t input_span = channel_maximum[0] - channel_minimum[0];
    const uint16_t output_span = channel_maximum[1] - channel_minimum[1];
    uint32_t attenuation = 0u;
    if (input_span >= MIN_SIGNAL_SPAN) {
        attenuation = ((uint32_t)output_span * 100u + input_span / 2u)
            / input_span;
        if (attenuation > 999u) attenuation = 999u;
    }

    char status[24];
    snprintf(status, sizeof(status), "ATT %lu%% 500k", (unsigned long)attenuation);

    ssd1306_clear(display);
    ssd1306_draw_string(display, 0, 0, 1, status);

    draw_channel(
        display,
        channel_samples[0],
        first[0],
        &display_scale[0],
        CH1_GRAPH_TOP,
        CH1_GRAPH_BOTTOM);
    draw_channel(
        display,
        channel_samples[1],
        first[1],
        &display_scale[1],
        CH2_GRAPH_TOP,
        CH2_GRAPH_BOTTOM);

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
    adc_gpio_init(ADC0_GPIO_PIN);
    adc_gpio_init(ADC1_GPIO_PIN);
    adc_select_input(ADC0_INPUT);
    adc_set_round_robin(0u);
    adc_fifo_setup(true, true, 1, false, false);

    /* El ADC recibe 48 MHz. Cada muestra empieza cada (1 + divisor) ciclos. */
    const float adc_divider =
        (float)clock_get_hz(clk_adc) / (float)ADC_SAMPLE_RATE_HZ - 1.0f;
    adc_set_clkdiv(adc_divider);

    const uint dma_channel = dma_claim_unused_channel(true);
    dma_channel_config dma_config = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_config, false);
    channel_config_set_write_increment(&dma_config, true);
    channel_config_set_dreq(&dma_config, DREQ_ADC);

    while (true) {
        capture_both_channels(dma_channel, &dma_config);
        draw_capture(&display);
    }
}
