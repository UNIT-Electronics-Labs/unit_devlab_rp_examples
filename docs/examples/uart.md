# Práctica 2: Comunicación UART

Esta práctica implementa comunicación serial asíncrona (UART) para transmitir y recibir datos entre el RP2040 y una computadora.

## Objetivos

- Configurar el periférico UART del RP2040
- Transmitir datos por puerto serial
- Recibir y procesar comandos desde terminal
- Implementar un shell interactivo básico
- Usar interrupciones UART

## Material Necesario

- Placa con RP2040 (Raspberry Pi Pico)
- Cable USB (para UART por USB)
- Terminal serial (PuTTY, minicom, screen, o Arduino Serial Monitor)

## UART del RP2040

El RP2040 tiene dos periféricos UART (UART0 y UART1):

- **UART0**: GP0 (TX), GP1 (RX)
- **UART1**: GP4 (TX), GP5 (RX)

Alternativamente, se puede usar **UART sobre USB** con `stdio_usb`.

## Código en C

### Ejemplo 1: Transmisión Básica (USB)

```c
#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    // Inicializar USB-UART
    stdio_init_all();
    
    printf("RP2040 UART Example\n");
    printf("Iniciando...\n");
    
    int contador = 0;
    
    while (true) {
        printf("Contador: %d\n", contador++);
        sleep_ms(1000);
    }
}
```

### Ejemplo 2: Echo (Repetir lo Recibido)

```c
#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    
    printf("=== Echo UART ===\n");
    printf("Escribe algo:\n");
    
    while (true) {
        int c = getchar_timeout_us(0);  // No bloqueante
        
        if (c != PICO_ERROR_TIMEOUT) {
            printf("Recibido: '%c' (0x%02X)\n", c, c);
            putchar(c);  // Echo
        }
    }
}
```

### Ejemplo 3: Shell Interactivo

```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN 25
#define BUFFER_SIZE 64

void process_command(char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        printf("Comandos disponibles:\n");
        printf("  help   - Mostrar ayuda\n");
        printf("  led on - Encender LED\n");
        printf("  led off- Apagar LED\n");
        printf("  status - Ver estado\n");
        
    } else if (strcmp(cmd, "led on") == 0) {
        gpio_put(LED_PIN, 1);
        printf("LED encendido\n");
        
    } else if (strcmp(cmd, "led off") == 0) {
        gpio_put(LED_PIN, 0);
        printf("LED apagado\n");
        
    } else if (strcmp(cmd, "status") == 0) {
        printf("LED: %s\n", gpio_get(LED_PIN) ? "ON" : "OFF");
        
    } else {
        printf("Comando desconocido: '%s'\n", cmd);
        printf("Escribe 'help' para ayuda\n");
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    char buffer[BUFFER_SIZE];
    int idx = 0;
    
    printf("\n=== RP2040 Shell ===\n");
    printf("Escribe 'help' para comandos\n");
    printf("> ");
    
    while (true) {
        int c = getchar_timeout_us(0);
        
        if (c != PICO_ERROR_TIMEOUT) {
            if (c == '\r' || c == '\n') {
                buffer[idx] = '\0';
                printf("\n");
                
                if (idx > 0) {
                    process_command(buffer);
                    idx = 0;
                }
                
                printf("> ");
                
            } else if (c == '\b' || c == 127) {  // Backspace
                if (idx > 0) {
                    idx--;
                    printf("\b \b");
                }
                
            } else if (idx < BUFFER_SIZE - 1) {
                buffer[idx++] = c;
                putchar(c);
            }
        }
    }
}
```

### Ejemplo 4: UART Hardware (UART0)

```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int main() {
    stdio_init_all();
    
    // Configurar UART0
    uart_init(UART_ID, BAUD_RATE);
    
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Configurar formato: 8N1
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    
    // Habilitar FIFO
    uart_set_fifo_enabled(UART_ID, true);
    
    const char *msg = "UART0 inicializado\r\n";
    uart_puts(UART_ID, msg);
    
    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            uart_putc(UART_ID, c);  // Echo
            
            printf("UART0 recibió: '%c'\n", c);
        }
        
        sleep_ms(10);
    }
}
```

### Ejemplo 5: UART con Interrupciones

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define BUFFER_SIZE 256
static char rx_buffer[BUFFER_SIZE];
static volatile int rx_head = 0;
static volatile int rx_tail = 0;

void uart_irq_handler() {
    while (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        
        int next_head = (rx_head + 1) % BUFFER_SIZE;
        if (next_head != rx_tail) {
            rx_buffer[rx_head] = c;
            rx_head = next_head;
        }
    }
}

bool buffer_available() {
    return rx_head != rx_tail;
}

char buffer_getc() {
    if (!buffer_available()) {
        return 0;
    }
    
    char c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % BUFFER_SIZE;
    return c;
}

int main() {
    stdio_init_all();
    
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Configurar interrupción RX
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, uart_irq_handler);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);  // RX enable, TX disable
    
    printf("UART con interrupciones inicializado\n");
    
    while (true) {
        if (buffer_available()) {
            char c = buffer_getc();
            printf("Recibido: '%c'\n", c);
            uart_putc(UART_ID, c);  // Echo
        }
        
        // Hacer otras tareas aquí
        sleep_ms(100);
    }
}
```

## Código en Ensamblador

Acceso directo a registros UART:

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.equ UART0_BASE,    0x40034000
.equ UARTDR,        0x000
.equ UARTFR,        0x018
.equ FR_TXFF,       (1 << 5)
.equ FR_RXFE,       (1 << 4)

.section .text
.global uart_putc_asm
.type uart_putc_asm, %function

// void uart_putc_asm(char c)
// R0 = carácter a transmitir
uart_putc_asm:
    ldr r1, =UART0_BASE
    
wait_tx:
    ldr r2, [r1, #UARTFR]
    movs r3, #FR_TXFF
    tst r2, r3
    bne wait_tx
    
    str r0, [r1, #UARTDR]
    bx lr

.global uart_getc_asm
.type uart_getc_asm, %function

// char uart_getc_asm(void)
// Retorna en R0
uart_getc_asm:
    ldr r1, =UART0_BASE
    
wait_rx:
    ldr r2, [r1, #UARTFR]
    movs r3, #FR_RXFE
    tst r2, r3
    bne wait_rx
    
    ldr r0, [r1, #UARTDR]
    bx lr

.size uart_putc_asm, . - uart_putc_asm
.size uart_getc_asm, . - uart_getc_asm
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(uart_example C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(uart_example
    main.c
)

target_link_libraries(uart_example
    pico_stdlib
    hardware_uart
)

pico_add_extra_outputs(uart_example)

# UART por USB
pico_enable_stdio_usb(uart_example 1)
pico_enable_stdio_uart(uart_example 0)
```

## Uso

### Compilar

```bash
mkdir build && cd build
cmake ..
make
```

### Conectar

#### Linux

```bash
minicom -D /dev/ttyACM0 -b 115200
# o
screen /dev/ttyACM0 115200
```

#### macOS

```bash
screen /dev/tty.usbmodem* 115200
```

#### Windows

Usar PuTTY:
- Connection type: Serial
- Serial line: COM3 (verificar en Device Manager)
- Speed: 115200

## Ejercicios Propuestos

1. **Parser de comandos**: Implementar comandos con argumentos (ej. `led 1 on`)
2. **Formato JSON**: Enviar/recibir datos en formato JSON
3. **Protocolo binario**: Implementar comunicación binaria con checksum
4. **Bridge UART**: Conectar UART0 y UART1, retransmitir datos entre ellos

## Conceptos Clave

- **Baudrate**: Velocidad de transmisión en bits por segundo
- **8N1**: 8 bits de datos, sin paridad, 1 bit de stop
- **FIFO**: Buffer interno para almacenar datos
- **Blocking vs Non-blocking**: Funciones que esperan vs retornan inmediatamente
- **IRQ**: Interrupt Request (manejo de datos en segundo plano)

## Referencias

- [RP2040 Datasheet - Capítulo 4.2: UART](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico SDK - hardware_uart](https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__uart.html)
