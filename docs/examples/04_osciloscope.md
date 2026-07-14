# Osciloscopio OLED de un canal

Esta aplicacion combina el ADC del RP2040, el controlador DMA y una pantalla OLED SSD1306 para construir un osciloscopio digital de un canal. A diferencia del sensor de temperatura interno visto en la practica de ADC, aqui el objetivo es capturar y representar graficamente una senal analogica externa (0 V a 3.3 V) con una tasa de muestreo fija, un mecanismo de disparo (*trigger*) y una escala vertical que se ajusta de forma automatica a la amplitud de la senal.

Un osciloscopio de banco resuelve tres problemas al mismo tiempo: adquiere muestras a un ritmo constante, decide en que punto de la captura "empieza" a dibujar (para que la imagen no parpadee de forma erratica) y escala la representacion para aprovechar todo el area visible. Esta aplicacion resuelve los tres con las herramientas ya vistas en Perifericos Basicos, mas un componente nuevo: el controlador DMA.

## Concepto teorico

### Por que DMA y no un bucle de lectura

En la practica de ADC se leia el resultado de una conversion de forma sincronica, sondeando el registro de resultado. Ese enfoque es adecuado para una sola lectura ocasional, pero es inviable a 500 kS/s: el CPU tendria que atender una interrupcion o sondear un registro cada 2 microsegundos, sin margen para hacer nada mas.

El controlador **DMA** (*Direct Memory Access*) resuelve esto moviendo datos entre un periferico y la memoria sin intervencion del CPU en cada transferencia. Un canal DMA se configura una sola vez con: direccion origen, direccion destino, tamano del dato, si cada direccion se incrementa entre transferencias, y una senal de solicitud (*DREQ*) que le indica al canal cuando el periferico tiene un dato listo. Configurado el canal, el ADC "empuja" cada muestra a su FIFO interno, el DREQ del ADC notifica al canal DMA, y el canal copia el dato a un arreglo en RAM de forma autonoma. El CPU se entera del final de la transferencia por sondeo de una bandera o por bloqueo explicito, y mientras tanto queda libre.

Esta aplicacion usa un patron de **captura por bloque** (*block capture*): el canal DMA se configura para mover un numero fijo de muestras (1024) y detenerse; el CPU inicia el ADC, espera a que el bloque se complete, detiene el ADC y procesa el bloque completo antes de iniciar la siguiente captura. Es el mismo principio que usa un osciloscopio de banco en modo *single/normal*: se llena un buffer completo antes de refrescar la pantalla, lo que evita que la actualizacion de la OLED por I2C (mas lenta que el muestreo) interfiera con el tiempo de adquisicion.

### Disparo (trigger) por histeresis

Si cada bloque capturado se dibujara desde su primera muestra, una senal periodica pareceria "deslizarse" horizontalmente en cada refresco, porque el instante en que arranca la captura no guarda relacion con la fase de la senal. Un osciloscopio real resuelve esto con un **disparo**: busca un punto de referencia dentro del bloque capturado (tipicamente un cruce por cierto nivel) y dibuja siempre a partir de ese punto.

Esta aplicacion implementa un disparo por **flanco ascendente con histeresis de dos niveles**, calculados como porcentajes del rango detectado en el propio bloque (minimo y maximo del bloque, con el 45% y el 55% de su amplitud como umbral bajo y alto). El algoritmo se "arma" cuando la senal cae por debajo del umbral bajo, y dispara en el primer cruce del umbral alto estando armado. La histeresis (dos umbrales distintos, en vez de uno solo) evita que el ruido alrededor de un unico nivel de cruce genere disparos falsos repetidos. Si la amplitud del bloque es demasiado pequena (senal plana o ausente), el disparo se omite y se dibuja desde el inicio del bloque.

### Autoescala con expansion rapida y contraccion lenta

La escala vertical (que rango de codigos ADC ocupa toda la altura de la grafica) se recalcula en cada bloque a partir del minimo y maximo detectados, con un margen adicional para que la traza no toque los bordes. Para evitar que la escala "salte" de forma brusca entre capturas sucesivas, la expansion (cuando la nueva senal es mas amplia que la escala actual) se aplica de inmediato, mientras que la contraccion (cuando la senal se vuelve mas pequena) se aplica de forma gradual mediante un filtro de promedio ponderado (7 partes del valor anterior por 1 parte del nuevo valor). El resultado es una escala que reacciona rapido ante una senal que crece, pero no oscila de forma nerviosa cuando la senal se reduce momentaneamente.

## Hardware y conexiones

| Senal | Pin RP2040 | Destino |
|---|---|---|
| Entrada analogica | GP26 (ADC0) | Senal a medir (0 V a 3.3 V) |
| I2C SDA | GP0 | SSD1306, pin SDA |
| I2C SCL | GP1 | SSD1306, pin SCL |
| GND | GND | Referencia comun (senal, OLED y RP2040) |

> La entrada analogica del RP2040 admite unicamente el rango de 0 V a 3.3 V. Cualquier senal fuera de ese rango requiere acondicionamiento externo (atenuador o desplazamiento de nivel) antes de conectarse a GP26.

## Dependencias de software

| Elemento | Detalle |
|---|---|
| Biblioteca | `daschr/pico-ssd1306` |
| Autor | David Schramm |
| Licencia | MIT |
| Repositorio | https://github.com/daschr/pico-ssd1306 |
| Funciones de API usadas | `ssd1306_init`, `ssd1306_clear`, `ssd1306_draw_pixel`, `ssd1306_draw_line`, `ssd1306_draw_string`, `ssd1306_show` |

Las funciones `hardware_adc`, `hardware_dma` y `hardware_i2c` pertenecen al Pico SDK oficial y se documentan como parte del codigo fuente, igual que en las practicas de Perifericos Basicos.

## Configuracion del proyecto — CMake

```cmake
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lib/pico-ssd1306
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_adc
    hardware_dma
    hardware_i2c
    pico_ssd1306
)
```

`PROJECT_SOURCES` incluye `main.c` y los archivos fuente de `pico-ssd1306` (`ssd1306.c` y su encabezado), agregados como submodulo o copia local dentro de `lib/`.

## Codigo fuente

```c
/**
 * @file main.c
 * @author obviousfancy
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
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
```

## Analisis del codigo

Esta seccion cubre unicamente los elementos nuevos respecto a las practicas de ADC e I2C ya vistas.

**`adc_fifo_setup(true, true, 1, false, false)`**: habilita el FIFO del ADC (primer parametro), habilita que el DREQ se genere cuando el FIFO tiene al menos un dato (segundo parametro), fija el umbral del DREQ en 1 muestra (tercer parametro), deshabilita el bit de error por dato (`err_in_fifo`, cuarto parametro, no aplicable a un unico canal fijo) y deshabilita el reductor a 8 bits (quinto parametro, se conserva la resolucion completa de 12 bits para mayor precision vertical).

**`dma_claim_unused_channel(true)`**: reserva un canal DMA libre del sistema; el parametro `true` indica que, si no hay canales disponibles, el programa debe detenerse con un mensaje de panico en vez de continuar con un canal invalido.

**`channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16)`**: cada transferencia mueve 16 bits, el ancho del resultado del ADC (aunque util solo usa 12).

**`channel_config_set_read_increment(&dma_config, false)`**: la direccion de lectura no avanza entre transferencias, porque siempre se lee del mismo registro (`&adc_hw->fifo`).

**`channel_config_set_write_increment(&dma_config, true)`**: la direccion de escritura si avanza, para llenar el arreglo `samples[]` de forma secuencial.

**`channel_config_set_dreq(&dma_config, DREQ_ADC)`**: el canal solo transfiere un dato cuando el ADC senala, mediante su DREQ especifico, que hay una muestra lista en el FIFO.

**`dma_channel_configure(...)`** (dentro de `capture_block`): aplica la configuracion anterior a un canal concreto, indicando origen (`&adc_hw->fifo`), destino (`samples`), numero de transferencias (`CAPTURE_SAMPLES`) y si debe iniciar de inmediato (`true`). Se reconfigura en cada bloque porque el canal se detiene automaticamente al completar el conteo de transferencias.

**`dma_channel_wait_for_finish_blocking`**: bloquea la ejecucion hasta que el canal termina las 1024 transferencias solicitadas; es el equivalente, para DMA, del `sleep_ms` bloqueante usado en practicas anteriores para esperar un evento.

**Disparo (`find_trigger`) y autoescala (`update_display_scale`)**: logica de aplicacion pura en C estandar, sin registros del RP2040 involucrados; se documentan en el Concepto Teorico.

## Verificacion

Al conectar una senal periodica (por ejemplo, la salida de la practica de PWM, o un generador de funciones externo limitado a 3.3 V) en GP26, la pantalla OLED debe mostrar una traza estable, sin deslizamiento horizontal entre refrescos, ocupando la mayor parte del area vertical disponible. El encabezado de la pantalla indica la tasa de muestreo (500 kS/s) y el tiempo total de captura visible en pantalla (1.02 ms para 512 muestras representadas en 128 columnas).

<div align="center"><img src="../examples/resources/02_stdio.png" alt="Traza estable de una senal periodica en el osciloscopio OLED"></div>

## Errores comunes y variantes

| Sintoma | Causa tipica |
|---|---|
| Pantalla en blanco o `ssd1306_init` retorna falso | Conexion I2C incorrecta (SDA/SCL invertidos) o direccion distinta a 0x3C; verificar con el escaner de bus de la practica de I2C |
| Traza recortada en la parte superior o inferior | Senal fuera del rango 0 V a 3.3 V; requiere acondicionamiento externo antes de GP26 |
| Traza que se desliza horizontalmente entre refrescos | Amplitud de la senal menor a `MIN_SIGNAL_SPAN`, por lo que el disparo se omite; verificar amplitud o reducir el umbral |
| Imagen congelada o el programa no avanza | El canal DMA reservado con `dma_claim_unused_channel(true)` no esta disponible (todos los canales en uso por otro periferico) |

**Variantes:**

1. Agregar un segundo canal de entrada (por ejemplo, GP27 / ADC1) alternando `adc_select_input` entre capturas, para mostrar dos senales superpuestas con distinto color de trazo (o distinto simbolo, dado que la OLED es monocromatica).
2. Sustituir el disparo por flanco ascendente por uno configurable por flanco descendente, agregando una variable que seleccione el modo mediante un boton (retomando la practica de GPIO).
3. Anadir un modo de "captura congelada": al presionar un boton, el bucle principal deja de llamar a `capture_block` y conserva la ultima traza en pantalla hasta que se presiona de nuevo.
