# Ejemplos y Prácticas

Este repositorio agrupa ejemplos y prácticas de programación de microcontroladores utilizando el **RP2040** como plataforma de desarrollo. El RP2040 es un microcontrolador dual-core ARM Cortex-M0+ desarrollado por Raspberry Pi.

## Contenido Temático

### Arquitectura y Programación
- Arquitecturas de microcontroladores de propósito general
- Implementación utilizando RP2040 (arquitectura Cortex-M0+)
- Programación en lenguaje ensamblador
- Programación en lenguaje C

### Periféricos y Comunicaciones
- Entradas y salidas digitales (GPIO)
- Comunicación UART
- Comunicación I²C
- Comunicación SPI
- ADC (Conversor Analógico-Digital)
- Temporizadores

### Herramientas de Desarrollo
- SDK de RP2040
- Compiladores y depuradores
- Herramientas de programación y carga

## Prácticas de Laboratorio

### 1. [Lectura de Entradas Digitales (GPIO)](./gpio.md)

Práctica introductoria para configurar y leer entradas digitales GPIO del RP2040. Incluye:
- Configuración de pines como entrada y salida
- Lectura de estados digitales y control de LEDs
- Debouncing de botones por software
- Uso de interrupciones GPIO

[📖 Ver práctica completa →](./gpio.md)

### 2. [Comunicación UART](./uart.md)

Implementación de comunicación serial asíncrona (UART):
- Configuración de baudrate y formato
- Transmisión y recepción de datos
- Shell interactivo por terminal
- Manejo de interrupciones UART

[📖 Ver práctica completa →](./uart.md)

### 3. [Lectura de Temperatura mediante ADC](./adc.md)

Uso del conversor analógico-digital para medición de temperatura:
- Configuración del ADC de 12 bits
- Lectura del sensor de temperatura interno del RP2040
- Conversión de valores ADC a unidades físicas
- Muestreo, promediado y uso de DMA

[📖 Ver práctica completa →](./adc.md)

### 4. [Comunicación I²C con Display OLED](./i2c-oled.md)

Interfaz I²C para control de display OLED:
- Configuración de bus I²C (SDA, SCL)
- Protocolo de comunicación I²C
- Control de display OLED SSD1306
- Visualización de texto, gráficos y animaciones

[📖 Ver práctica completa →](./i2c-oled.md)

## Flujo Recomendado

```bash
# Compilar ejemplo
mkdir build
cd build
cmake ..
make

# Cargar a la placa
# Mantener BOOTSEL presionado y conectar USB
# Copiar el archivo .uf2 generado
```

Para programación directa con picotool:

```bash
picotool load example.uf2
picotool reboot
```
