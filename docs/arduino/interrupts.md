# Interrupciones en el RP2040: Fundamentos y Programación en Arduino

## Introducción

El manejo de eventos externos es uno de los aspectos que con mayor frecuencia distingue una implementación eficiente de una implementación ineficiente en sistemas embebidos. Mientras que el sondeo continuo del estado de un pin (*polling*) consume ciclos de CPU de manera constante, el mecanismo de interrupciones permite que el procesador permanezca ejecutando su tarea principal y sea desviado únicamente cuando ocurre un evento relevante. Este documento describe el mecanismo de interrupciones del RP2040 desde su fundamento arquitectónico en el núcleo Cortex-M0+ hasta su uso práctico mediante la capa de abstracción que ofrece Arduino (core `arduino-pico`).

## 1. Sondeo frente a interrupciones

| Característica | Sondeo (*polling*) | Interrupción |
|---|---|---|
| Uso de CPU | Continuo, aun sin eventos | Nulo hasta que ocurre el evento |
| Latencia de respuesta | Depende de la frecuencia del sondeo | Determinística, del orden de ciclos de reloj |
| Complejidad de implementación | Baja | Media (requiere disciplina en el diseño de la rutina de servicio) |
| Escalabilidad ante múltiples eventos asíncronos | Limitada | Adecuada |

El sondeo resulta apropiado cuando el evento a detectar ocurre con una cadencia conocida y compatible con el ciclo del programa principal. Cuando el evento es asíncrono —como la activación de un sensor externo en un instante arbitrario— el uso de interrupciones resulta la aproximación correcta.

## 2. El controlador de interrupciones del Cortex-M0+: NVIC

Cada núcleo del RP2040 integra un controlador de interrupciones vectorizado y anidado estándar de ARM, denominado NVIC (*Nested Vectored Interrupt Controller*). El NVIC admite hasta 32 entradas de interrupción, numeradas de 0 a 31; en el caso particular del RP2040, únicamente las 26 líneas inferiores están conectadas a una fuente de hardware real (IRQ 0 a IRQ 25), mientras que las líneas 26 a 31 permanecen sin conexión.

La siguiente tabla enumera las 26 fuentes de interrupción disponibles en el RP2040:

| IRQ | Fuente |
|---|---|
| 0 | TIMER_IRQ_0 |
| 1 | TIMER_IRQ_1 |
| 2 | TIMER_IRQ_2 |
| 3 | TIMER_IRQ_3 |
| 4 | PWM_IRQ_WRAP |
| 5 | USBCTRL_IRQ |
| 6 | XIP_IRQ |
| 7 | PIO0_IRQ_0 |
| 8 | PIO0_IRQ_1 |
| 9 | PIO1_IRQ_0 |
| 10 | PIO1_IRQ_1 |
| 11 | DMA_IRQ_0 |
| 12 | DMA_IRQ_1 |
| 13 | IO_IRQ_BANK0 |
| 14 | IO_IRQ_QSPI |
| 15 | SIO_IRQ_PROC0 |
| 16 | SIO_IRQ_PROC1 |
| 17 | CLOCKS_IRQ |
| 18 | SPI0_IRQ |
| 19 | SPI1_IRQ |
| 20 | UART0_IRQ |
| 21 | UART1_IRQ |
| 22 | ADC0_IRQ_FIFO |
| 23 | I2C0_IRQ |
| 24 | I2C1_IRQ |
| 25 | RTC_IRQ |

Cada núcleo dispone de su propio NVIC, con estado de habilitación independiente. La mayoría de las líneas están duplicadas de forma simétrica entre ambos núcleos, con la salvedad de las interrupciones de GPIO, que se describen en la sección siguiente.

Respecto a prioridades, el campo de prioridad definido por la arquitectura ARM es de 8 bits, pero el Cortex-M0+ únicamente implementa los 2 bits más significativos de dicho campo, resultando en 4 niveles de prioridad efectivos. La convención utilizada asigna el valor numérico más bajo a la prioridad más alta. El carácter *anidado* del controlador (la "N" de NVIC) permite que una interrupción de mayor prioridad interrumpa la rutina de servicio de una interrupción de menor prioridad ya en ejecución.

## 3. Caso particular: las interrupciones de GPIO (`IO_IRQ_BANK0`)

A diferencia de periféricos como UART, I²C o los temporizadores —cada uno con su propia línea dedicada en el NVIC—, las interrupciones de los 30 pines GPIO del banco 0 se canalizan hacia una única línea compartida por núcleo: `IO_IRQ_BANK0` (IRQ 13). El controlador de interrupciones no distingue, en esa única línea, cuál de los 30 pines originó el evento; esa determinación se resuelve mediante la lectura de registros de estado por pin dentro de la rutina de servicio compartida, la cual descarta o atiende el evento según corresponda y limpia la bandera correspondiente antes de retornar.

Esta particularidad ofrece un punto de comparación directo con la arquitectura de interrupciones de la familia PIC de gama media, donde tradicionalmente existe un único vector de interrupción (dirección 0004h) compartido por todos los periféricos, requiriendo que la rutina de servicio consulte los bits de bandera correspondientes (por ejemplo, en los registros `INTCON` o `PIRx`) para identificar el origen del evento. El mecanismo de demultiplexado de `IO_IRQ_BANK0` en el RP2040 responde a esa misma lógica para el caso específico de GPIO. La diferencia sustancial aparece en el resto de periféricos: mientras que en PIC gama media el mismo esquema de vector único y bandera aplica de forma generalizada, en el RP2040 UART, I²C, SPI, ADC, temporizadores, DMA y PIO cuentan cada uno con su propia entrada individual en el NVIC, reduciendo la necesidad de sondeo de banderas fuera del caso de GPIO.

## 4. La abstracción de Arduino: `attachInterrupt()`

El core `arduino-pico` expone el mecanismo de interrupciones mediante la misma interfaz estándar de Arduino, ocultando la gestión directa del NVIC y del demultiplexado descrito en la sección anterior.

```cpp
attachInterrupt(digitalPinToInterrupt(PIN), funcionISR, MODO);
```

- **`digitalPinToInterrupt(PIN)`**: en placas AVR de 8 bits tradicionales (por ejemplo, Arduino Uno), únicamente un subconjunto reducido de pines admite interrupciones externas (D2 y D3). En el RP2040, dado que todos los pines GPIO comparten el mismo mecanismo de detección de flanco o nivel a través de `IO_IRQ_BANK0`, cualquier GPIO puede emplearse como fuente de interrupción sin restricción de asignación fija.
- **`funcionISR`**: referencia a la función de servicio. Debe pasarse el nombre de la función, sin paréntesis.
- **`MODO`**: condición de disparo, según la tabla siguiente.

| Modo | Condición de disparo |
|---|---|
| `RISING` | Flanco de subida (transición de LOW a HIGH) |
| `FALLING` | Flanco de bajada (transición de HIGH a LOW) |
| `CHANGE` | Cualquier cambio de estado (ambos flancos) |
| `HIGH` | Mientras el pin permanezca en nivel alto |
| `LOW` | Mientras el pin permanezca en nivel bajo |

**Advertencia sobre los modos por nivel (`HIGH`/`LOW`)**: a diferencia de los modos por flanco, que disparan la interrupción una única vez por transición, los modos por nivel generan disparos repetidos mientras la condición se mantenga activa. Si la rutina de servicio no modifica esa condición (por ejemplo, deshabilitando temporalmente la interrupción) y la señal permanece en ese nivel por un periodo prolongado, el procesador puede quedar saturado atendiendo la misma interrupción de manera continua.

## 5. Buenas prácticas para el diseño de una rutina de servicio (ISR)

- La ISR no debe recibir parámetros ni retornar valores.
- Debe mantenerse lo más breve y rápida posible; el procesador suspende la ejecución del programa principal durante toda su duración.
- `delay()` no funciona dentro de una ISR, dado que depende de interrupciones para operar. `millis()` no avanza mientras se ejecuta la ISR. `micros()` mantiene su valor únicamente durante 1 o 2 milisegundos antes de volverse errático. `delayMicroseconds()`, al no depender de un contador de interrupciones, sí opera con normalidad.
- Toda variable compartida entre la ISR y el programa principal debe declararse `volatile`, indicando al compilador que su valor puede modificarse de forma asíncrona y evitando optimizaciones que asuman un valor constante entre lecturas.
- El uso de `Serial.print()` dentro de una ISR —presente en los ejemplos introductorios de este seminario con fines de depuración— constituye una operación bloqueante y no determinística que debe evitarse en código de producción; su función aquí es exclusivamente didáctica.

**Nota sobre atomicidad**: en arquitecturas de 8 bits como PIC gama media, la lectura de una variable de 16 o 32 bits modificada por una ISR requiere precauciones adicionales (deshabilitar interrupciones durante la lectura, o realizar doble lectura), dado que la operación se descompone en múltiples accesos de 8 bits y puede quedar interrumpida a la mitad. En el RP2040, al tratarse de una arquitectura de 32 bits con bus de datos nativo de ese ancho, la lectura o escritura de una variable de tipo `unsigned long` (32 bits) correctamente alineada se resuelve en una única instrucción de carga o almacenamiento, por lo que resulta atómica respecto a interrupciones en el mismo núcleo. Esta garantía no se extiende a tipos de mayor tamaño (`long long`, `double`) ni a accesos concurrentes entre ambos núcleos del RP2040.

## 6. Nota de compatibilidad: la macro `IRAM_ATTR`

Es frecuente encontrar ejemplos migrados desde ESP32 que declaran la rutina de servicio como `void IRAM_ATTR nombreFuncion()`. En el ESP32, `IRAM_ATTR` instruye al enlazador para ubicar la función en memoria IRAM en lugar de en flash, evitando fallos de ejecución cuando la caché de flash se invalida durante ciertas operaciones (por ejemplo, escritura en flash o gestión de Wi-Fi).

El RP2040 no presenta esa restricción de la misma manera, por lo que la macro `IRAM_ATTR` carece de efecto funcional en esta arquitectura. Sin embargo, su presencia o ausencia en el código sí tiene consecuencia práctica sobre la compilación: el core `arduino-pico` únicamente define `IRAM_ATTR` como una macro vacía dentro de encabezados específicos vinculados a compatibilidad de red (por ejemplo, al utilizar ciertas bibliotecas de mDNS o WiFi). En un sketch base que no incluya dichos encabezados, `IRAM_ATTR` no está definido en ningún punto, y el compilador interpreta el token como un identificador inesperado, produciendo un error de compilación del tipo `expected initializer before 'nombreFuncion'`.

La recomendación para código dirigido específicamente al RP2040 es omitir `IRAM_ATTR` por completo, dado que no aporta beneficio alguno en esta arquitectura y su inclusión depende de bibliotecas accesorias que pueden no estar presentes.

## 7. Ejemplo aplicado: conteo de eventos mediante un sensor de salida digital

El siguiente ejemplo ilustra los conceptos anteriores aplicados a un sensor capacitivo con salida digital ya resuelta (el sensor entrega un nivel HIGH cuando detecta activación y LOW en reposo; la determinación capacitiva ocurre internamente en el módulo del sensor, no en el RP2040).

```cpp
#include <Arduino.h>

const int SENSOR_PIN = 7;              // GP7
volatile unsigned long pulseCount = 0;

void onPulse() {
  pulseCount++;
  Serial.print("Pulso detectado. Total: ");
  Serial.println(pulseCount);
}

void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), onPulse, RISING);
  Serial.println("Esperando pulsos del sensor en GP7...");
}

void loop() {
  delay(1000);
}
```

Se emplea el identificador `SENSOR_PIN` en lugar de `TOUCH_PIN` de manera deliberada: el RP2040 no posee un periférico de sensado capacitivo nativo equivalente al de ciertas líneas de ESP32, por lo que nombrar la variable en función de "touch" podría sugerir, incorrectamente, que el microcontrolador realiza esa función. El pin únicamente recibe una señal digital ya procesada por el sensor externo.

## 8. Síntesis comparativa

| Aspecto | PIC gama media | RP2040 (Cortex-M0+) |
|---|---|---|
| Vectores de interrupción | Único (0004h), compartido por todos los periféricos | 26 líneas conectadas en el NVIC, con vector individual por periférico (excepto GPIO) |
| Identificación del origen | Sondeo de banderas (`INTCON`, `PIRx`) dentro de la ISR única | Vector dedicado por periférico; sondeo de banderas solo en el caso de GPIO (`IO_IRQ_BANK0`) |
| Niveles de prioridad | Variable según el dispositivo (alta/baja en PIC18, ausente en PIC16) | 4 niveles, configurables por interrupción |
| Pines habilitados para interrupción externa | Subconjunto fijo (INT0, INT1, RBx) | Cualquier GPIO |
| Atomicidad de una variable de 32 bits | Requiere protección explícita | Atómica de forma nativa en un mismo núcleo |

## Referencias

- Raspberry Pi Ltd. *RP2040 Datasheet*, sección 2.3.2 "Interrupts". <https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf>
- Raspberry Pi (Trading) Ltd. *Pico SDK — hardware_irq*. <https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#hardware_irq>
- Earle F. Philhower, III. *Arduino-Pico Documentation — Digital I/O*. <https://arduino-pico.readthedocs.io/en/latest/digital.html>
- Arduino. *Language Reference — attachInterrupt()*. <https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/>
