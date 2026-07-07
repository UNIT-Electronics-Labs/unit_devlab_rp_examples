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

### <span id="gpio">1. Lectura de Entradas Digitales</span>

Práctica introductoria para configurar y leer entradas digitales GPIO del RP2040. Incluye:
- Configuración de pines como entrada
- Lectura de estados digitales
- Control de LEDs según entradas
- Debouncing de botones

### <span id="uart">2. Comunicación UART</span>

Implementación de comunicación serial asíncrona (UART):
- Configuración de baudrate
- Transmisión y recepción de datos
- Comunicación con terminal serie
- Manejo de interrupciones UART

### <span id="adc">3. Lectura de Temperatura mediante ADC</span>

Uso del conversor analógico-digital para medición de temperatura:
- Configuración del ADC interno
- Lectura del sensor de temperatura interno del RP2040
- Conversión de valores ADC a temperatura
- Muestreo y promediado de lecturas

### <span id="i2c">4. Comunicación I²C con Display OLED</span>

Interfaz I²C para control de display OLED:
- Configuración de bus I²C (SDA, SCL)
- Protocolo de comunicación I²C
- Control de display OLED (ej. SSD1306)
- Visualización de texto y gráficos

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
