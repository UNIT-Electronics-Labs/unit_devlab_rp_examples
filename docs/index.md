---
layout: home

hero:
  name: RP2040 Labs
  text: Ejemplos y Prácticas para RP2040
  tagline: Documentación práctica para programar el microcontrolador RP2040 en ensamblador y C, con ejemplos de comunicación UART, I²C, SPI, ADC y más.
  actions:
    - theme: brand
      text: Ver ejemplos
      link: /examples/
    - theme: alt
      text: Guía de inicio
      link: /guide/devlab

features:
  - title: Arquitectura Cortex-M0+
    details: Implementación práctica utilizando RP2040 como plataforma compatible con arquitectura ARM Cortex-M0+.
  - title: Ensamblador y C
    details: Ejemplos en lenguaje ensamblador y C para entender la programación de bajo nivel en microcontroladores.
  - title: Periféricos completos
    details: Prácticas con GPIO, UART, I²C, SPI, ADC y temporizadores del RP2040.
  - title: Herramientas de desarrollo
    details: Flujo de desarrollo moderno con SDK oficial y herramientas open-source.
---

## Contenido Temático

- **Arquitecturas de microcontroladores** de propósito general
- Implementación utilizando **RP2040** como plataforma compatible con arquitectura Cortex-M0+
- Programación en **lenguaje ensamblador**
- Programación en **lenguaje C**
- **Entradas y salidas digitales**
- **Comunicación UART**
- **Comunicación I²C**
- **Comunicación SPI**
- **ADC y temporizadores**
- Herramientas de desarrollo para microcontroladores

## Prácticas de Laboratorio

- [Lectura de entradas digitales](./examples/gpio.md)
- [Comunicación UART](./examples/uart.md)
- [Lectura de temperatura mediante ADC](./examples/adc.md)
- [Comunicación I²C con display OLED](./examples/i2c-oled.md)

## Guías de Desarrollo

- [Configuración del SDK](./guide/devlab.md)
- [Guía para Windows](./guide/windows.md)
- [Programación en C](./guide/c-programming.md)
- [Programación en Ensamblador](./guide/asm-programming.md)

## Uso Básico

```bash
npm install
npm run docs:dev
```

El sitio se sirve localmente con VitePress y documenta los ejemplos que viven en este repositorio.
