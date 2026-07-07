# Práctica 3: Lectura de Temperatura mediante ADC

Esta práctica utiliza el Conversor Analógico-Digital (ADC) del RP2040 para leer el sensor de temperatura interno y entradas analógicas externas.

## Objetivos

- Configurar y utilizar el ADC del RP2040
- Leer el sensor de temperatura interno
- Convertir valores ADC a unidades físicas
- Implementar muestreo y promediado
- Leer entradas analógicas externas

## El ADC del RP2040

El RP2040 tiene un ADC de 12 bits (valores 0-4095) con:
- **4 canales externos**: GP26 (ADC0), GP27 (ADC1), GP28 (ADC2), GP29 (ADC3)
- **1 canal interno**: Sensor de temperatura (ADC4)
- Voltaje de referencia: 3.3V
- Frecuencia de muestreo: hasta 500 kS/s

## Material Necesario

- Placa con RP2040 (Raspberry Pi Pico)
- Potenciómetro 10kΩ (opcional, para entrada analógica)
- Sensor de temperatura externo como LM35 o TMP36 (opcional)

## Código en C

### Ejemplo 1: Lectura del Sensor Interno

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main() {
    stdio_init_all();
    
    // Inicializar ADC
    adc_init();
    
    // Habilitar sensor de temperatura interno
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);  // Canal 4 = sensor de temperatura
    
    printf("=== Sensor de Temperatura RP2040 ===\n");
    
    while (true) {
        // Leer valor ADC (0-4095)
        uint16_t raw = adc_read();
        
        // Convertir a voltaje
        const float conversion_factor = 3.3f / 4095.0f;
        float voltage = raw * conversion_factor;
        
        // Convertir a temperatura (fórmula del datasheet)
        // T = 27 - (ADC_voltage - 0.706)/0.001721
        float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
        
        printf("Raw: %4d | Voltaje: %.3fV | Temp: %.2f°C\n", 
               raw, voltage, temperature);
        
        sleep_ms(1000);
    }
}
```

### Ejemplo 2: Lectura con Promediado

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define NUM_SAMPLES 16

float read_temperature_averaged() {
    uint32_t sum = 0;
    
    // Tomar múltiples muestras
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += adc_read();
        sleep_us(100);  // Pequeño delay entre muestras
    }
    
    // Calcular promedio
    uint16_t avg = sum / NUM_SAMPLES;
    
    // Convertir a temperatura
    const float conversion = 3.3f / 4095.0f;
    float voltage = avg * conversion;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

int main() {
    stdio_init_all();
    
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    
    printf("=== Temperatura con Promediado ===\n");
    
    while (true) {
        float temp = read_temperature_averaged();
        printf("Temperatura: %.2f°C\n", temp);
        sleep_ms(1000);
    }
}
```

### Ejemplo 3: Múltiples Canales (Temperatura + Potenciómetro)

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define POT_PIN 26  // GP26 = ADC0

void init_adc_pins() {
    adc_init();
    
    // Configurar GP26 como entrada ADC
    adc_gpio_init(POT_PIN);
    
    // Habilitar sensor de temperatura
    adc_set_temp_sensor_enabled(true);
}

float read_channel(uint8_t channel) {
    adc_select_input(channel);
    uint16_t raw = adc_read();
    return raw * (3.3f / 4095.0f);
}

int main() {
    stdio_init_all();
    init_adc_pins();
    
    printf("=== Múltiples Canales ADC ===\n");
    
    while (true) {
        // Leer potenciómetro (canal 0)
        float pot_voltage = read_channel(0);
        float pot_percent = (pot_voltage / 3.3f) * 100.0f;
        
        // Leer temperatura (canal 4)
        float temp_voltage = read_channel(4);
        float temperature = 27.0f - (temp_voltage - 0.706f) / 0.001721f;
        
        printf("Potenciómetro: %.2fV (%5.1f%%) | Temperatura: %.2f°C\n",
               pot_voltage, pot_percent, temperature);
        
        sleep_ms(500);
    }
}
```

### Ejemplo 4: Alarma de Temperatura

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_PIN 25
#define TEMP_THRESHOLD 28.0f  // °C

float read_temperature() {
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * (3.3f / 4095.0f);
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    adc_init();
    adc_set_temp_sensor_enabled(true);
    
    printf("=== Alarma de Temperatura ===\n");
    printf("Umbral: %.1f°C\n", TEMP_THRESHOLD);
    
    while (true) {
        float temp = read_temperature();
        
        if (temp > TEMP_THRESHOLD) {
            gpio_put(LED_PIN, 1);
            printf("⚠️  ALERTA: %.2f°C (LED ON)\n", temp);
        } else {
            gpio_put(LED_PIN, 0);
            printf("✓ Normal: %.2f°C\n", temp);
        }
        
        sleep_ms(1000);
    }
}
```

### Ejemplo 5: DMA y Free-Running Mode

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#define CAPTURE_DEPTH 1000

uint16_t capture_buf[CAPTURE_DEPTH];

void setup_adc_dma() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    
    // Configurar ADC para free-running
    adc_fifo_setup(
        true,    // Habilitar FIFO
        true,    // Habilitar DMA
        1,       // Umbral de DREQ
        false,   // Sin error
        false    // No reducir a 8 bits
    );
    
    // Configurar DMA
    int dma_chan = dma_claim_unused_channel(true);
    
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_ADC);
    
    dma_channel_configure(
        dma_chan,
        &cfg,
        capture_buf,        // dst
        &adc_hw->fifo,      // src
        CAPTURE_DEPTH,      // transfer count
        false               // no start yet
    );
    
    // Iniciar ADC en free-running
    adc_run(true);
    
    // Iniciar DMA
    dma_channel_start(dma_chan);
    
    // Esperar a que complete
    dma_channel_wait_for_finish_blocking(dma_chan);
    
    adc_run(false);
    adc_fifo_drain();
}

int main() {
    stdio_init_all();
    
    printf("=== ADC con DMA ===\n");
    printf("Capturando %d muestras...\n", CAPTURE_DEPTH);
    
    setup_adc_dma();
    
    // Procesar datos
    uint32_t sum = 0;
    uint16_t min = 4095;
    uint16_t max = 0;
    
    for (int i = 0; i < CAPTURE_DEPTH; i++) {
        uint16_t val = capture_buf[i];
        sum += val;
        if (val < min) min = val;
        if (val > max) max = val;
    }
    
    float avg = (float)sum / CAPTURE_DEPTH;
    float avg_voltage = avg * (3.3f / 4095.0f);
    float avg_temp = 27.0f - (avg_voltage - 0.706f) / 0.001721f;
    
    printf("Muestras capturadas: %d\n", CAPTURE_DEPTH);
    printf("Promedio: %.2f (%.2f°C)\n", avg, avg_temp);
    printf("Rango: %d - %d\n", min, max);
    
    while (true) {
        tight_loop_contents();
    }
}
```

## Código en Ensamblador

Acceso directo a registros ADC:

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.equ ADC_BASE,      0x4004c000
.equ ADC_CS,        0x00
.equ ADC_RESULT,    0x04
.equ ADC_FCS,       0x08
.equ CS_START_ONCE, (1 << 2)
.equ CS_READY,      (1 << 8)

.section .text
.global adc_read_asm
.type adc_read_asm, %function

// uint16_t adc_read_asm(void)
adc_read_asm:
    ldr r0, =ADC_BASE
    
    // Iniciar conversión
    ldr r1, [r0, #ADC_CS]
    movs r2, #CS_START_ONCE
    orrs r1, r2
    str r1, [r0, #ADC_CS]
    
wait_ready:
    ldr r1, [r0, #ADC_CS]
    lsrs r1, #9          // Verificar bit READY
    bcc wait_ready
    
    // Leer resultado
    ldr r0, [r0, #ADC_RESULT]
    bx lr

.size adc_read_asm, . - adc_read_asm
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(adc_example C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(adc_example
    main.c
)

target_link_libraries(adc_example
    pico_stdlib
    hardware_adc
    hardware_dma
)

pico_add_extra_outputs(adc_example)
pico_enable_stdio_usb(adc_example 1)
pico_enable_stdio_uart(adc_example 0)
```

## Diagrama de Conexiones (Opcional)

```
RP2040 Pico:
  GP26 (ADC0) ← Potenciómetro central
  3V3 → Potenciómetro extremo
  GND → Potenciómetro extremo
  
  GP27 (ADC1) ← TMP36 Vout
  3V3 → TMP36 Vs+
  GND → TMP36 GND
```

## Ejercicios Propuestos

1. **Datalogger**: Guardar lecturas de temperatura con timestamp
2. **Control PID**: Implementar control de temperatura con LED/ventilador
3. **Display**: Mostrar temperatura en display LCD o OLED via I²C
4. **Calibración**: Implementar calibración de offset para el sensor
5. **Múltiples sensores**: Leer 4 canales ADC y mostrar todos

## Conceptos Clave

- **ADC**: Analog-to-Digital Converter (Conversor Analógico-Digital)
- **Resolución**: 12 bits = 4096 niveles (0-4095)
- **Vref**: Voltaje de referencia (3.3V en RP2040)
- **Promediado**: Reducción de ruido mediante múltiples muestras
- **DMA**: Direct Memory Access (transferencia sin CPU)

## Referencias

- [RP2040 Datasheet - Capítulo 4.9: ADC](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico SDK - hardware_adc](https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__adc.html)
