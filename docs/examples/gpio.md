# Práctica 1: Lectura de Entradas Digitales (GPIO)

Esta práctica introduce la configuración y lectura de pines GPIO del RP2040, implementando un sistema de control de LED mediante botones.

## Objetivos

- Configurar pines GPIO como entrada y salida
- Implementar lectura de botones con pull-up/pull-down
- Controlar LEDs según el estado de entradas
- Aplicar debouncing por software

## Material Necesario

- Placa con RP2040 (Raspberry Pi Pico)
- 2 botones pulsadores
- 2 LEDs
- 2 resistencias de 220Ω (para LEDs)
- Protoboard y cables

## Diagrama de Conexiones

```
RP2040 Pico:
  GP25 (LED incorporado)
  GP15 → LED externo (+ resistencia 220Ω a GND)
  GP14 → Botón 1 (+ pull-down a GND)
  GP13 → Botón 2 (+ pull-down a GND)
```

## Código en C

### Ejemplo Básico

```c
#include "pico/stdlib.h"

#define LED_PIN 25
#define LED_EXT 15
#define BTN1 14
#define BTN2 13

int main() {
    // Inicializar pines de LED como salida
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_init(LED_EXT);
    gpio_set_dir(LED_EXT, GPIO_OUT);
    
    // Inicializar pines de botón como entrada
    gpio_init(BTN1);
    gpio_set_dir(BTN1, GPIO_IN);
    gpio_pull_down(BTN1);  // Resistencia pull-down interna
    
    gpio_init(BTN2);
    gpio_set_dir(BTN2, GPIO_IN);
    gpio_pull_down(BTN2);
    
    // Loop principal
    while (true) {
        // Leer estado de botones
        bool btn1_pressed = gpio_get(BTN1);
        bool btn2_pressed = gpio_get(BTN2);
        
        // Controlar LEDs según botones
        gpio_put(LED_PIN, btn1_pressed);
        gpio_put(LED_EXT, btn2_pressed);
        
        sleep_ms(10);  // Pequeño delay para debouncing
    }
}
```

### Ejemplo con Debouncing

```c
#include "pico/stdlib.h"
#include <stdio.h>

#define LED_PIN 25
#define BTN1 14
#define DEBOUNCE_MS 50

typedef struct {
    uint pin;
    bool last_state;
    absolute_time_t last_change;
} button_t;

bool button_read_debounced(button_t *btn) {
    bool current = gpio_get(btn->pin);
    absolute_time_t now = get_absolute_time();
    
    if (current != btn->last_state) {
        if (absolute_time_diff_us(btn->last_change, now) > DEBOUNCE_MS * 1000) {
            btn->last_state = current;
            btn->last_change = now;
            return current;
        }
    }
    
    return btn->last_state;
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_init(BTN1);
    gpio_set_dir(BTN1, GPIO_IN);
    gpio_pull_down(BTN1);
    
    button_t boton = {
        .pin = BTN1,
        .last_state = false,
        .last_change = get_absolute_time()
    };
    
    bool led_state = false;
    
    while (true) {
        bool btn = button_read_debounced(&boton);
        
        // Toggle LED en flanco ascendente
        static bool last_btn = false;
        if (btn && !last_btn) {
            led_state = !led_state;
            gpio_put(LED_PIN, led_state);
            printf("LED: %s\n", led_state ? "ON" : "OFF");
        }
        last_btn = btn;
        
        sleep_ms(10);
    }
}
```

### Ejemplo con Interrupciones

```c
#include "pico/stdlib.h"
#include <stdio.h>

#define LED_PIN 25
#define BTN1 14

volatile bool led_state = false;

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BTN1 && (events & GPIO_IRQ_EDGE_RISE)) {
        led_state = !led_state;
        gpio_put(LED_PIN, led_state);
        printf("LED toggled: %s\n", led_state ? "ON" : "OFF");
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_init(BTN1);
    gpio_set_dir(BTN1, GPIO_IN);
    gpio_pull_down(BTN1);
    
    // Habilitar interrupción en flanco ascendente
    gpio_set_irq_enabled_with_callback(BTN1, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    
    while (true) {
        tight_loop_contents();
    }
}
```

## Código en Ensamblador

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.equ SIO_BASE,      0xd0000000
.equ GPIO_IN,       0x004
.equ GPIO_OUT_SET,  0x014
.equ GPIO_OUT_CLR,  0x018
.equ LED_PIN,       25
.equ BTN_PIN,       14

.section .text
.global gpio_example
.type gpio_example, %function

gpio_example:
    push {r4-r6, lr}
    
    ldr r4, =SIO_BASE
    movs r5, #1
    lsls r5, r5, #LED_PIN      // R5 = máscara LED
    
    movs r6, #1
    lsls r6, r6, #BTN_PIN      // R6 = máscara botón
    
loop:
    // Leer estado de GPIO
    ldr r0, [r4, #GPIO_IN]
    
    // Verificar botón
    tst r0, r6
    beq led_off
    
led_on:
    str r5, [r4, #GPIO_OUT_SET]
    b delay
    
led_off:
    str r5, [r4, #GPIO_OUT_CLR]
    
delay:
    ldr r0, =10000
    bl delay_cycles
    b loop

delay_cycles:
    subs r0, #1
    bne delay_cycles
    bx lr

.size gpio_example, . - gpio_example
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(gpio_example C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(gpio_example
    main.c
)

target_link_libraries(gpio_example
    pico_stdlib
)

pico_add_extra_outputs(gpio_example)
pico_enable_stdio_usb(gpio_example 1)
pico_enable_stdio_uart(gpio_example 0)
```

## Ejercicios Propuestos

1. **Semáforo**: Implementar un semáforo con 3 LEDs (rojo, amarillo, verde) que cambie de estado con un botón
2. **Contador binario**: Mostrar un contador de 0-15 en 4 LEDs, incrementando con un botón
3. **Antirrebote hardware**: Agregar un capacitor de 100nF en paralelo con el botón
4. **Múltiples botones**: Leer 4 botones y mostrar el estado en 4 LEDs independientes

## Conceptos Clave

- **Pull-up/Pull-down**: Resistencias que establecen un estado definido cuando el botón no está presionado
- **Debouncing**: Eliminación de rebotes mecánicos del botón
- **Interrupciones**: Respuesta inmediata a eventos sin polling constante
- **GPIO**: General Purpose Input/Output (Entrada/Salida de Propósito General)

## Referencias

- [RP2040 Datasheet - Capítulo 2.19: GPIO](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico SDK - hardware_gpio](https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__gpio.html)
