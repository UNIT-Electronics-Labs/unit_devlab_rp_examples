# Ejemplo 01: Lectura de entradas digitales

Corresponde a la práctica **Lectura de entradas digitales** del Módulo II. Lee el estado de un botón con pull-up interno, refleja el estado en el LED de la placa, y reporta cada cambio por consola USB — de paso, sirve para ver el patrón de debounce por software más simple posible.

## Conexiones

| Señal | Pin | Notas |
|---|---|---|
| Botón | GPIO 14 | Un extremo al pin, el otro a GND. Sin resistencia externa: se usa el pull-up interno |
| LED integrado | GPIO 25 | Prende mientras el botón está presionado |

## Estructura del proyecto

```
01_lectura_entradas_digitales/
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── main.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(lectura_entradas)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(lectura_entradas
    main.c
)

target_link_libraries(lectura_entradas
    pico_stdlib
)

pico_add_extra_outputs(lectura_entradas)
pico_enable_stdio_usb(lectura_entradas 1)
pico_enable_stdio_uart(lectura_entradas 0)
```

## main.c

```c
#include "pico/stdlib.h"
#include <stdio.h>

#define LED_PIN    25
#define BUTTON_PIN 14

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // botón a GND: presionado = 0

    bool estado_anterior = true;  // true = no presionado (pull-up)

    while (true) {
        bool estado_actual = gpio_get(BUTTON_PIN);

        if (estado_actual != estado_anterior) {
            sleep_ms(20);  // debounce: espera a que se estabilice
            estado_actual = gpio_get(BUTTON_PIN);

            if (estado_actual != estado_anterior) {
                gpio_put(LED_PIN, !estado_actual);
                printf("Boton %s\n", estado_actual ? "liberado" : "presionado");
                estado_anterior = estado_actual;
            }
        }
    }
}
```

## Explicación

Con pull-up interno el pin lee `1` en reposo y `0` cuando el botón conecta a GND — por eso la lógica está invertida (`!estado_actual` para el LED). El debounce aquí es deliberadamente simple: al detectar un cambio, espera 20 ms y vuelve a leer; si el cambio persiste, se considera real. Es suficiente para un botón de panel, pero no es la única forma — la alternativa por interrupciones está en la [guía de C, sección Interrupciones GPIO](../../guide/c-programming.md#interrupciones-gpio), y vale la pena mostrarla como contraste una vez que este ejemplo ya funciona.

## Compilar y cargar

```bash
mkdir build && cd build
cmake ..
make
```

Ver [configuración del entorno](../../guide/devlab.md#cargar-el-programa) para el flasheo.

## Verificación

Abre una terminal serial sobre el puerto USB-CDC que expone la placa (p. ej. `/dev/ttyACM0` en Linux) a 115200 baudios. Cada vez que presiones o sueltes el botón debe imprimirse una línea, y el LED debe seguir el estado en tiempo real.

## Variantes

- Reemplazar el polling por `gpio_set_irq_enabled_with_callback` y comparar consumo de CPU.
- Agregar un contador de pulsaciones y mostrarlo cada vez que cambia.
- Usar el botón para alternar la frecuencia de parpadeo del [ejemplo 00](./blink.md) en vez de un simple encendido/apagado.
- Combinar con PWM (`hardware_pwm`) para que el LED se atenúe gradualmente en vez de encender de golpe.

## Errores comunes

| Síntoma | Causa típica |
|---|---|
| El pin lee estados aleatorios sin tocar el botón | Falta `gpio_pull_up()` — el pin queda flotando |
| No se ve nada en la terminal | `pico_enable_stdio_usb` no está en `1`, o se está viendo el puerto equivocado |
| El LED parpadea solo al soltar, nunca al presionar | Lógica invertida: revisar el signo en `gpio_put(LED_PIN, !estado_actual)` |
