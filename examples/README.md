# Ejemplos para RP2040

Este directorio contiene ejemplos prácticos de programación para el microcontrolador RP2040.

## Estructura de Ejemplos

Cada ejemplo incluye:
- Código fuente en C o ensamblador
- CMakeLists.txt para compilación
- picodev.toml para compilación y programación con picodev
- Documentación específica del ejemplo
- Instrucciones de uso

## Prácticas Disponibles

### GPIO - Entradas Digitales
Lectura de entradas digitales y control de salidas.

### UART - Comunicación Serial
Transmisión y recepción de datos por puerto serial.

### ADC - Sensor de Temperatura
Lectura del sensor de temperatura interno mediante ADC.

### I2C - Display OLED
Comunicación I²C con display OLED para visualización de datos.

## Requisitos

- Placa con RP2040 (Raspberry Pi Pico, etc.)
- SDK de Pico (pico-sdk)
- CMake 3.20 o superior
- Compilador ARM GCC

## Compilación

### Con picodev

Instala las herramientas una sola vez:

```bash
pip install picodev
picodev install
```

Después entra al directorio de cualquier ejemplo y compílalo:

```bash
cd 00_blink_practice
picodev build
```

Para programar la placa mediante una sonda SWD:

```bash
picodev flash
```

Todos los ejemplos usan el importador compartido
`examples/pico_sdk_import.cmake`. No es necesario copiar ese archivo dentro de
cada proyecto mientras se conserve esta estructura del repositorio.

### Con CMake

```bash
mkdir build
cd build
cmake ..
make
```

## Carga del Programa

1. Mantener presionado el botón BOOTSEL
2. Conectar la placa por USB
3. Soltar BOOTSEL
4. Copiar el archivo .uf2 al dispositivo de almacenamiento que aparece

O usar picotool:

```bash
picotool load -f example.uf2
picotool reboot
```

## Documentación

Consulta la [documentación completa](../docs/index.md) para más detalles sobre cada práctica.
