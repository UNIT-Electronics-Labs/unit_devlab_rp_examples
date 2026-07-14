# Pico C/C++ SDK
 
Esta seccion es el nucleo del curso: el manejo del **RP2040** (Raspberry Pi Pico) directamente con las funciones oficiales del **Pico SDK 2.2.0**, sin pasar por Arduino. Se divide en dos rutas complementarias:
 
- **Periféricos Básicos** — 12 practicas secuenciales, cada una limitada a un solo periferico: inicializacion, asignacion de pines, configuracion elemental y un ciclo de validacion basico.
- **Aplicaciones e Integración** — proyectos que combinan varios perifericos entre si (y, en algunos casos, hardware o bibliotecas de terceros) para resolver un problema completo.


## Periféricos Básicos
 
| # | Práctica | Contenido |
|---|---|---|
| 1 | [Blink](../examples/basic/00_blink.md) | `gpio_init`, `gpio_put`, `sleep_ms`, CMake basico |
| 2 | [Serial](../examples/basic/01_stdio.md) | `stdio_init_all` + `printf` por USB |
| 3 | [GPIO](../examples/basic/02_gpio.md) | Entrada de boton, pull-up interno, antirrebote por *polling* |
| 4 | [IRQ](../examples/basic/03_irq.md) | Interrupciones externas, flancos, callbacks |
| 5 | [ADC](../examples/basic/04_adc.md) | Conversion analogica, sensor de temperatura interno (canal 4) |
| 6 | [PWM](../examples/basic/05_pwm.md) | Slices, ciclo de trabajo, divisores de reloj |
| 7 | [Timer](../examples/basic/06_timer.md) | `repeating_timer`, alarmas asincronas no bloqueantes |
| 8 | [UART](../examples/basic/07_uart.md) | Baudrate, TX/RX, eco real contra el CH340 del shield |
| 9 | [I2C](../examples/basic/08_i2c.md) | Escaner de bus (sin OLED, diferido a Aplicaciones) |
| 10 | [SPI](../examples/basic/09_spi.md) | Verificacion de `CHIP_ID` del BMI270 real, sin biblioteca |
| 11 | [Multicore](../examples/basic/10_multicore.md) | Core 1, FIFOs entre nucleos |
| 12 | [PIO](../examples/basic/11_pio.md) | Maquinas de estado, ensamblador `.pio` basico |

### Herramientas de Desarrollo
- SDK de RP2040
- Compiladores y depuradores
- Herramientas de programación y carga

## Prácticas de Laboratorio

## Aplicaciones e Integración
 
Proyectos completos que integran varios de los perifericos anteriores con hardware real del shield (sensores externos, display, IMU) o funcionalidad avanzada del RP2040 (DMA).
 
### 1. [Sensor de Temperatura Interno con Despliegue en OLED](./01_OledTempInt.md)
 
Lectura del sensor de temperatura interno por ADC, con despliegue del valor en un display OLED SSD1306 via I2C (biblioteca `pico-ssd1306` de David Schramm). Primera integracion ADC + I2C de la seccion.
 
### 2. [Control de Brillo de un LED mediante Potenciometro (ADC + PWM + OLED)](./02_adcpwm.md)
 
Un potenciometro externo, leido por el ADC, controla en tiempo real el ciclo de trabajo de una senal PWM que modula el brillo de un LED. El valor crudo, el porcentaje y el voltaje estimado se muestran de forma simultanea en el OLED, con una tasa de refresco desacoplada de la del par ADC-PWM.
 
### 3. [IMU BMI270 por SPI con Despliegue en OLED](./03_oledbmi270.md)
 
Inicializacion completa del BMI270 (secuencia oficial de Bosch, incluida la carga del archivo de configuracion de 8192 bytes) y lectura de acelerometro y giroscopio por SPI, con los valores en las tres unidades desplegados en el OLED y por consola serial.
 
### 4. [Osciloscopio OLED de un Canal](./04_osciloscope.md)
 
Captura de una senal analogica externa mediante ADC y DMA en modo de bloque, con disparo (*trigger*) por histeresis de dos niveles y autoescala vertical, dibujando el trazo resultante en el OLED — el equivalente funcional, en miniatura, de un osciloscopio de banco.

## Flujo Recomendado

```bash
picodev-flash
```

