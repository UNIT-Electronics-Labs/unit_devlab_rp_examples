# Práctica 4: Comunicación I²C con Display OLED

Esta práctica implementa comunicación I²C para controlar un display OLED (típicamente SSD1306 o SH1106) y mostrar texto y gráficos.

## Objetivos

- Configurar el bus I²C del RP2040
- Implementar protocolo de comunicación I²C
- Controlar un display OLED
- Visualizar texto y gráficos
- Integrar con sensor de temperatura (ADC + I²C)

## Material Necesario

- Placa con RP2040 (Raspberry Pi Pico)
- Display OLED 128x64 con controlador SSD1306 o SH1106 (I²C)
- Cables jumper
- Resistencias pull-up 4.7kΩ (normalmente ya incluidas en el módulo OLED)

## El Bus I²C del RP2040

El RP2040 tiene dos periféricos I²C (I2C0 e I2C1):

- **I2C0**: GP4 (SDA), GP5 (SCL) - pines por defecto
- **I2C1**: GP6 (SDA), GP7 (SCL) - pines por defecto

Otros pines compatibles según multiplexado GPIO.

## Diagrama de Conexiones

```
Display OLED → RP2040 Pico
  VCC → 3.3V
  GND → GND
  SDA → GP4 (I2C0 SDA)
  SCL → GP5 (I2C0 SCL)
```

**Nota**: La mayoría de módulos OLED ya incluyen resistencias pull-up de 4.7kΩ o 10kΩ.

## Código en C

### Ejemplo 1: Escanear Bus I²C

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

void i2c_scan() {
    printf("\n=== Escaneo de Bus I2C ===\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    
    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
        
        // Intentar escribir 0 bytes
        int ret;
        uint8_t rxdata;
        ret = i2c_read_blocking(I2C_PORT, addr, &rxdata, 1, false);
        
        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("@ = Dispositivo encontrado\n");
}

int main() {
    stdio_init_all();
    
    // Inicializar I2C a 400kHz
    i2c_init(I2C_PORT, 400 * 1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    sleep_ms(1000);
    i2c_scan();
    
    while (true) {
        tight_loop_contents();
    }
}
```

### Ejemplo 2: Control Básico SSD1306

```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

static uint8_t oled_buffer[OLED_WIDTH * OLED_HEIGHT / 8];

void oled_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
}

void oled_send_data(uint8_t data) {
    uint8_t buf[2] = {0x40, data};
    i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
}

void oled_init() {
    oled_send_cmd(0xAE);  // Display OFF
    oled_send_cmd(0xD5);  // Set display clock
    oled_send_cmd(0x80);
    oled_send_cmd(0xA8);  // Set multiplex
    oled_send_cmd(0x3F);
    oled_send_cmd(0xD3);  // Set display offset
    oled_send_cmd(0x00);
    oled_send_cmd(0x40);  // Set start line
    oled_send_cmd(0x8D);  // Charge pump
    oled_send_cmd(0x14);
    oled_send_cmd(0x20);  // Memory mode
    oled_send_cmd(0x00);  // Horizontal
    oled_send_cmd(0xA1);  // Segment remap
    oled_send_cmd(0xC8);  // COM scan direction
    oled_send_cmd(0xDA);  // COM pins
    oled_send_cmd(0x12);
    oled_send_cmd(0x81);  // Contrast
    oled_send_cmd(0xCF);
    oled_send_cmd(0xD9);  // Pre-charge
    oled_send_cmd(0xF1);
    oled_send_cmd(0xDB);  // VCOMH
    oled_send_cmd(0x40);
    oled_send_cmd(0xA4);  // Display all on resume
    oled_send_cmd(0xA6);  // Normal display
    oled_send_cmd(0xAF);  // Display ON
}

void oled_clear() {
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

void oled_update() {
    oled_send_cmd(0x21);  // Column address
    oled_send_cmd(0);
    oled_send_cmd(127);
    oled_send_cmd(0x22);  // Page address
    oled_send_cmd(0);
    oled_send_cmd(7);
    
    for (int i = 0; i < sizeof(oled_buffer); i++) {
        oled_send_data(oled_buffer[i]);
    }
}

void oled_set_pixel(int x, int y, bool on) {
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) {
        return;
    }
    
    if (on) {
        oled_buffer[x + (y / 8) * OLED_WIDTH] |= (1 << (y % 8));
    } else {
        oled_buffer[x + (y / 8) * OLED_WIDTH] &= ~(1 << (y % 8));
    }
}

void oled_draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        oled_set_pixel(x0, y0, true);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

int main() {
    stdio_init_all();
    
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    
    sleep_ms(250);
    
    printf("Inicializando OLED...\n");
    oled_init();
    
    oled_clear();
    
    // Dibujar un rectángulo
    oled_draw_line(10, 10, 117, 10);
    oled_draw_line(117, 10, 117, 53);
    oled_draw_line(117, 53, 10, 53);
    oled_draw_line(10, 53, 10, 10);
    
    // Diagonal
    oled_draw_line(10, 10, 117, 53);
    
    oled_update();
    
    printf("Display actualizado\n");
    
    while (true) {
        tight_loop_contents();
    }
}
```

### Ejemplo 3: Display con Texto (Font 5x7)

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Font 5x7 básico (solo mayúsculas y dígitos)
static const uint8_t font5x7[][5] = {
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    // ... (continuar con resto del alfabeto)
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    // ... (continuar con 4-9)
};

void oled_draw_char(int x, int y, char c) {
    int index;
    if (c >= 'A' && c <= 'Z') {
        index = c - 'A';
    } else if (c >= '0' && c <= '9') {
        index = 26 + (c - '0');
    } else {
        return;  // Carácter no soportado
    }
    
    for (int col = 0; col < 5; col++) {
        uint8_t line = font5x7[index][col];
        for (int row = 0; row < 8; row++) {
            if (line & (1 << row)) {
                oled_set_pixel(x + col, y + row, true);
            }
        }
    }
}

void oled_draw_string(int x, int y, const char *str) {
    int cursor_x = x;
    while (*str) {
        oled_draw_char(cursor_x, y, *str);
        cursor_x += 6;  // 5 + espacio de 1 pixel
        str++;
    }
}

// ... (incluir funciones anteriores: oled_init, oled_update, etc.)

int main() {
    // ... (inicialización I2C)
    
    oled_init();
    oled_clear();
    
    oled_draw_string(10, 10, "HELLO");
    oled_draw_string(10, 30, "RP2040");
    
    oled_update();
    
    while (true) {
        tight_loop_contents();
    }
}
```

### Ejemplo 4: Integración ADC + I²C (Mostrar Temperatura)

```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

// Incluir funciones OLED de ejemplos anteriores

float read_temperature() {
    adc_select_input(4);
    
    uint32_t sum = 0;
    for (int i = 0; i < 16; i++) {
        sum += adc_read();
        sleep_us(100);
    }
    
    uint16_t avg = sum / 16;
    float voltage = avg * (3.3f / 4095.0f);
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

void format_temp_string(char *buf, float temp) {
    int temp_int = (int)temp;
    int temp_dec = (int)((temp - temp_int) * 10);
    sprintf(buf, "TEMP %d%dC", temp_int, temp_dec);
}

int main() {
    stdio_init_all();
    
    // Inicializar I2C
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    
    // Inicializar ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);
    
    sleep_ms(250);
    
    oled_init();
    
    char temp_str[20];
    
    while (true) {
        float temp = read_temperature();
        
        oled_clear();
        oled_draw_string(10, 10, "RP2040");
        
        format_temp_string(temp_str, temp);
        oled_draw_string(10, 30, temp_str);
        
        oled_update();
        
        printf("Temperatura: %.2f°C\n", temp);
        sleep_ms(1000);
    }
}
```

### Ejemplo 5: Animación Simple

```c
void oled_draw_circle(int cx, int cy, int r) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                oled_set_pixel(cx + x, cy + y, true);
            }
        }
    }
}

int main() {
    // ... inicialización
    
    oled_init();
    
    int x = 10;
    int dx = 2;
    
    while (true) {
        oled_clear();
        oled_draw_circle(x, 32, 8);
        oled_update();
        
        x += dx;
        if (x > 118 || x < 10) dx = -dx;
        
        sleep_ms(50);
    }
}
```

## Código en Ensamblador

Transmisión I²C básica:

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.equ I2C0_BASE,     0x40044000
.equ IC_DATA_CMD,   0x10
.equ IC_STATUS,     0x70
.equ STAT_TFNF,     (1 << 1)

.section .text
.global i2c_write_byte_asm
.type i2c_write_byte_asm, %function

// void i2c_write_byte_asm(uint8_t data)
i2c_write_byte_asm:
    push {r4, lr}
    movs r4, r0
    
    ldr r1, =I2C0_BASE
    
wait_fifo:
    ldr r2, [r1, #IC_STATUS]
    movs r3, #STAT_TFNF
    tst r2, r3
    beq wait_fifo
    
    str r4, [r1, #IC_DATA_CMD]
    
    pop {r4, pc}

.size i2c_write_byte_asm, . - i2c_write_byte_asm
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(i2c_oled C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(i2c_oled
    main.c
)

target_link_libraries(i2c_oled
    pico_stdlib
    hardware_i2c
    hardware_adc
)

pico_add_extra_outputs(i2c_oled)
pico_enable_stdio_usb(i2c_oled 1)
pico_enable_stdio_uart(i2c_oled 0)
```

## Librería SSD1306 Completa

Para proyectos reales, considera usar una librería existente:

```bash
# Clonar librería SSD1306
cd ~/pico
git clone https://github.com/daschr/pico-ssd1306.git
```

En CMakeLists.txt:
```cmake
add_subdirectory(../pico-ssd1306 ssd1306)

target_link_libraries(mi_proyecto
    ssd1306
)
```

## Ejercicios Propuestos

1. **Menú interactivo**: Control del display via UART
2. **Gráficos en tiempo real**: Mostrar gráfica de temperatura
3. **Reloj**: Implementar reloj digital con RTC o timer
4. **Juego simple**: Snake o Pong en el OLED
5. **Múltiples sensores**: Display con I²C + sensores I²C (BME280, MPU6050)

## Conceptos Clave

- **I²C**: Inter-Integrated Circuit (bus serial de 2 hilos)
- **SDA**: Serial Data (datos bidireccionales)
- **SCL**: Serial Clock (reloj generado por master)
- **Address**: Dirección del dispositivo (típicamente 0x3C o 0x3D para SSD1306)
- **Pull-up**: Resistencias que mantienen líneas en HIGH cuando inactivas
- **Frame buffer**: Memoria local que se transfiere al display

## Troubleshooting

- **Display no responde**: Verificar dirección I²C con escaneo (puede ser 0x3C o 0x3D)
- **Pantalla en blanco**: Verificar comando 0xAF (Display ON)
- **Imagen invertida**: Ajustar comandos 0xA1 y 0xC8
- **Errores I²C**: Verificar pull-ups, velocidad del bus, conexiones

## Referencias

- [RP2040 Datasheet - Capítulo 4.3: I2C](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico SDK - hardware_i2c](https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__i2c.html)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
