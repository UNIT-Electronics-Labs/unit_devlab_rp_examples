# Ejemplos y Prácticas

Este repositorio agrupa ejemplos y prácticas de programación de microcontroladores utilizando el microcontrolador **RP2040**  en su version Rapberry Pi Pico como plataforma de desarrollo. El RP2040 es un microcontrolador dual-core ARM Cortex-M0+ desarrollado por Raspberry Pi.

## Contenido Temático

### Arquitectura y Programación
- Arquitecturas de microcontroladores de propósito general
- Implementación utilizando RP2040 (arquitectura Cortex-M0+)
- Programación en lenguaje ensamblador
- Programación en lenguaje C
- Programación en Arduino IDE

### Periféricos y Comunicaciones
- Basicos RP2040
- Comunicacion USB
- Entradas y salidas digitales (GPIO)
- Interrupciones
- ADC (Conversor Analógico-Digital)
- PWM
- Temporizadores
- Comunicación UART
- Comunicación I²C
- Comunicación SPI
- Multinucleo Cortex M0
- PIO

### Herramientas de Desarrollo
- SDK de RP2040
- Compiladores y depuradores
- Herramientas de programación y carga

## Prácticas de Laboratorio

### 1. [Lectura de Entradas Digitales (GPIO)](./01_OledTempInt.md)

Práctica introductoria para configurar y leer entradas digitales GPIO del RP2040. Incluye:
- Configuración de pines como entrada y salida
- Lectura de estados digitales y control de LEDs
- Debouncing de botones por software
- Uso de interrupciones GPIO

[📖 Ver práctica completa →](./)

### 2. [Comunicación UART](./01_OledTempInt.md)

Implementación de comunicación serial asíncrona (UART):
- Configuración de baudrate y formato
- Transmisión y recepción de datos
- Shell interactivo por terminal
- Manejo de interrupciones UART

[📖 Ver práctica completa →](./adcoled.md)

### 3. [Lectura de Temperatura mediante ADC](./01_OledTempInt.md)

Uso del conversor analógico-digital para medición de temperatura:
- Configuración del ADC de 12 bits
- Lectura del sensor de temperatura interno del RP2040
- Conversión de valores ADC a unidades físicas
- Muestreo, promediado y uso de DMA

[📖 Ver práctica completa →](./adcoled.md)

### 4. [Comunicación I²C con Display OLED](./01_OledTempInt.md)

Interfaz I²C para control de display OLED:
- Configuración de bus I²C (SDA, SCL)
- Protocolo de comunicación I²C
- Control de display OLED SSD1306
- Visualización de texto, gráficos y animaciones

[📖 Ver práctica completa →](./01_OledTempInt.md)

## Flujo Recomendado

```bash
pico-flash
```

