# Programación en C para RP2040

Esta guía introduce la programación en C para el microcontrolador RP2040 utilizando el Pico SDK.

## Estructura Básica de un Proyecto
Un proyecto basico de SDK 
```shell
mi_proyecto/
├── mi_proyecto.c              # Código fuente principal
├── CMakeLists.txt             # Configuración de build
├── pico_sdk_import.cmake      # Localizador del SDK
└── build/                     # Generado por CMake (no versionar es interno en cada usuario)
    ├── mi_proyecto.elf
    ├── mi_proyecto.uf2
    ├── mi_proyecto.bin
    ├── mi_proyecto.hex
    └── CMakeCache.txt
```
### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

# Inicializar el SDK
include(pico_sdk_import.cmake)

project(mi_proyecto)


pico_sdk_init()

# Agregar ejecutable
add_executable(mi_programa
    main.c
)

# Vincular librerías
target_link_libraries(mi_programa
    pico_stdlib #libaditional
)

# Crear archivos de salida (UF2, BIN, HEX)
pico_add_extra_outputs(mi_programa)

# Habilitar UART por USB
pico_enable_stdio_usb(mi_programa 1)
pico_enable_stdio_uart(mi_programa 0)
```

### main.c básico

```c
/* User Includes */
#include "pico/stdlib.h"

/* User Defines */
#define MY_PIN 25

int main() {
    // Inicializar GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Loop principal
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}
```

## Librería Estándar del Pico SDK

### GPIO (pico_stdlib)

```c
#include "pico/stdlib.h"

// Inicializar pin
gpio_init(pin);

// Configurar dirección
gpio_set_dir(pin, GPIO_OUT);  // Salida
gpio_set_dir(pin, GPIO_IN);   // Entrada

// Escribir
gpio_put(pin, 1);  // HIGH
gpio_put(pin, 0);  // LOW

// Leer
bool estado = gpio_get(pin);

// Pull-up/Pull-down
gpio_pull_up(pin);
gpio_pull_down(pin);
```

### UART (hardware_uart)

```c
#include "hardware/uart.h"

// Inicializar UART0
uart_init(uart0, 115200);

// Configurar pines
gpio_set_function(0, GPIO_FUNC_UART);  // TX
gpio_set_function(1, GPIO_FUNC_UART);  // RX

// Transmitir
uart_puts(uart0, "Hola\n");
uart_putc(uart0, 'A');

// Recibir
if (uart_is_readable(uart0)) {
    char c = uart_getc(uart0);
}
```

### I²C (hardware_i2c)

```c
#include "hardware/i2c.h"

// Inicializar I2C0 a 400kHz
i2c_init(i2c0, 400000);

// Configurar pines
gpio_set_function(4, GPIO_FUNC_I2C);  // SDA
gpio_set_function(5, GPIO_FUNC_I2C);  // SCL
gpio_pull_up(4);
gpio_pull_up(5);

// Escribir a dispositivo
uint8_t data[] = {0x00, 0x3F};
i2c_write_blocking(i2c0, 0x3C, data, 2, false);

// Leer de dispositivo
uint8_t buffer[10];
i2c_read_blocking(i2c0, 0x3C, buffer, 10, false);
```

### SPI (hardware_spi)

```c
#include "hardware/spi.h"

// Inicializar SPI0 a 1MHz
spi_init(spi0, 1000000);

// Configurar pines
gpio_set_function(16, GPIO_FUNC_SPI);  // MISO
gpio_set_function(17, GPIO_FUNC_SPI);  // CS
gpio_set_function(18, GPIO_FUNC_SPI);  // SCK
gpio_set_function(19, GPIO_FUNC_SPI);  // MOSI

// Transmitir datos
uint8_t tx_data[] = {0x01, 0x02, 0x03};
spi_write_blocking(spi0, tx_data, 3);

// Recibir datos
uint8_t rx_data[10];
spi_read_blocking(spi0, 0, rx_data, 10);
```

### ADC (hardware_adc)

```c
#include "hardware/adc.h"

// Inicializar ADC
adc_init();

// Habilitar sensor de temperatura interno
adc_set_temp_sensor_enabled(true);
adc_select_input(4);  // Canal 4 = sensor temp

// Leer valor
uint16_t raw = adc_read();
float voltage = raw * 3.3f / 4096.0f;

// Convertir a temperatura
float temp = 27.0f - (voltage - 0.706f) / 0.001721f;
```

### Temporizadores (pico_time)

```c
#include "pico/time.h"

// Delays
sleep_ms(1000);    // 1 segundo
sleep_us(100);     // 100 microsegundos

// Obtener tiempo actual
absolute_time_t t = get_absolute_time();
uint64_t us = to_us_since_boot(t);

// Alarmas
bool timer_callback(repeating_timer_t *rt) {
    // Código a ejecutar
    return true;  // true = repetir
}

repeating_timer_t timer;
add_repeating_timer_ms(500, timer_callback, NULL, &timer);
```

## Interrupciones GPIO

```c
#include "hardware/gpio.h"
#include "hardware/irq.h"

void gpio_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_RISE) {
        // Flanco ascendente
    }
    if (events & GPIO_IRQ_EDGE_FALL) {
        // Flanco descendente
    }
}

int main() {
    gpio_init(2);
    gpio_set_dir(2, GPIO_IN);
    gpio_pull_up(2);
    
    // Habilitar interrupción
    gpio_set_irq_enabled_with_callback(2, 
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
        true, 
        &gpio_callback);
    
    while (1) tight_loop_contents();
}
```

## Compilación y Depuración

### Compilar

```bash
cmake -S . -B build
cmake --build build
openocd -f interface/cmsis-dap.cfg \
        -f target/rp2040.cfg \
        -c "adapter speed 5000" \
        -c "program build/${PROJECT_NAME}.elf verify reset exit"
```

### Mensajes de Debug por USB

```c
#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    
    while (true) {
        printf("Temperatura: %.2f°C\n", temp);
        sleep_ms(1000);
    }
}
```

### Proyecto Multi-Archivo

```cmake
add_executable(mi_programa
    main.c
    sensor.c
    display.c
)
```

```c
// sensor.h
#ifndef SENSOR_H
#define SENSOR_H

float leer_temperatura(void);

#endif
```

```c
// sensor.c
#include "sensor.h"
#include "hardware/adc.h"

float leer_temperatura(void) {
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096.0f;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}
```
### Proyecto Multilibreria

```cmake
target_link_libraries(lib1
    lib2
    libn
)
```
## Mejores Prácticas

1. **Siempre inicializar** periféricos antes de usar
2. **Usar constantes** para números de pin: `#define LED_PIN 25`
3. **Verificar retornos** de funciones I²C/SPI
4. **Documentar** el código con comentarios
5. **Modularizar** en archivos .h/.c separados
6. **Usar `const`** para valores que no cambian
7. **Evitar delays largos** en interrupciones

## Recursos

- [Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [C SDK Examples](https://github.com/raspberrypi/pico-examples)
