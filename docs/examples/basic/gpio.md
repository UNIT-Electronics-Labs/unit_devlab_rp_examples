# Ejemplo 02: Lectura de entradas digitales

Una vez validada, en el ejemplo anterior, la configuración del entorno de desarrollo y el control de un GPIO como salida digital, la presente práctica aborda el caso complementario: la lectura de una entrada digital. Se emplea un botón de panel conectado a un pin configurado como entrada, con resistencia de pull-up habilitada por software, para introducir dos elementos indispensables en la programación de bajo nivel: el muestreo periódico (polling) como mecanismo de detección de cambios de estado, y el criterio de antirrebote (debounce) necesario para descartar transiciones espurias generadas por el propio contacto mecánico del botón.

## Objetivo general
 
Configurar un pin GPIO del RP2040 como entrada digital con resistencia de pull-up interna, y determinar el estado de un botón mediante muestreo periódico, aplicando un criterio simple de antirrebote sobre la lectura obtenida.

## Hardware requerido
 
| Componente | Cantidad | Observaciones |
|---|---|---|
| Placa con RP2040 (Raspberry Pi Pico o equivalente) | 1 | Plataforma empleada en el Módulo II |
| Botón de panel (push-button) | 1 | Contacto normalmente abierto; no requiere resistencia externa |
 
## Conexiones
 
| Señal | Pin del RP2040 | Descripción |
|---|---|---|
| Botón | GPIO24 | Un extremo del botón se conecta al pin; el otro, a GND. Se utiliza el pull-up interno del RP2040, por lo que no se requiere resistencia externa |

## Estructura del proyecto
 
```
Practice_GPIO_02/
├── build
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── Practice_GPIO_02.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# ─────────────────────────────────────────────
# CONFIGURACIÓN DEL PROYECTO
# Generado automáticamente por pic0o-new
# ─────────────────────────────────────────────
set(PROJECT_NAME    "Practice_GPIO_O2")
set(PROJECT_SOURCES "Practice_GPIO_O2.c")
set(PICO_BOARD      "pico")

set(PROJECT_LIBS
    pico_stdlib
    hardware_gpio
)
# ─────────────────────────────────────────────
# NO MODIFICAR DE AQUÍ EN ADELANTE
# ─────────────────────────────────────────────

project(${PROJECT_NAME})

pico_sdk_init()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

```

## Código fuente — `Practice_GPIO_02.c`

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* ─── Defines ──────────────────────────────────────────── */
#define BUTTON_PIN 24

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    gpio_pull_up(BUTTON_PIN); // Pull up the button to avoid floating state the Chinese Board doesn't have pull up resistor in the circuit

    bool estado_anterior = gpio_get(BUTTON_PIN);

    
    if (estado_anterior) {
        printf("Button is not pressed\n");
    } else {
        printf("Button is pressed\n");
    }

    while (1) {

        bool estado_actual = gpio_get(BUTTON_PIN);

        if (estado_actual != estado_anterior) {
            
            if (estado_actual) {
                printf("Button is not pressed\n");
            } else {
                printf("Button is pressed\n");
            }

            estado_anterior = estado_actual;
        }
        sleep_ms(20); 
    }
    return 0;
}

```

## Explicación
 
Con el pull-up interno habilitado, el pin permanece en estado lógico `1` en reposo y transiciona a `0` cuando el botón conecta el pin a GND; por ello, la lógica de detección resulta invertida respecto a lo que podría esperarse de manera intuitiva. El criterio de antirrebote empleado es deliberadamente simple: al detectar un cambio de estado, se espera un intervalo de 20 ms antes de continuar el muestreo, dado que los rebotes mecánicos de un botón de panel suelen resolverse en un tiempo menor; de este modo, la siguiente lectura ya refleja el estado estable del contacto. Este mecanismo resulta suficiente para un botón de panel simple, aunque no es la única alternativa disponible: el uso de interrupciones GPIO se documenta, a manera de contraste, en la [guía de C, sección Interrupciones GPIO](../../guide/c-programming.md#interrupciones-gpio), y conviene presentarlo una vez que este ejemplo basado en muestreo periódico (polling) se encuentre ya validado.
 
## Errores comunes
 
| Síntoma | Causa típica |
|---|---|
| El pin lee estados aleatorios sin presionar el botón | Falta la llamada a `gpio_pull_up()`; el pin permanece en estado flotante |
| No se observa nada en la terminal | `pico_enable_stdio_usb` no está habilitado con el valor `1`, o se está monitoreando el puerto serial equivocado |
| El mensaje impreso no corresponde al estado físico del botón | Lógica invertida: revísese la comparación de `estado_actual`, considerando que el pull-up produce un `1` en reposo y un `0` al presionar |
 
## Compilación y carga
 
La programación de la placa se realiza mediante el depurador SWD (CH552), tal como se describe en el [ejemplo 00](./blink.md):
 
```bash
pico-flash
```
 
Consúltese la [configuración del entorno](../../guide/devlab.md#cargar-el-programa) para el detalle del procedimiento.
 
## Verificación
 
Ábrase una terminal serial sobre el puerto USB-CDC que expone la placa (por ejemplo, `/dev/ttyACM0` en Linux) a 115200 baudios:
 
```bash
minicom -b 115200 -D /dev/ttyACM0
```
 
Cada vez que se presione o se libere el botón debe imprimirse la línea correspondiente (`Button is pressed` o `Button is not pressed`), de manera inmediata y sin líneas duplicadas ni ausentes.
 
<div align="center">
  <img src="../resources/gpiopractice2.png" width="300px" alt="Mensajes Boton presionado y Boton liberado en terminal serial">
  <p><em>Salida esperada en la terminal serial</em></p>
</div>
## Variantes
 
- Agregar un contador de pulsaciones y mostrarlo cada vez que su valor cambie.
- Emplear el botón para alternar la frecuencia de parpadeo del [ejemplo 00](./blink.md), en lugar de un simple encendido/apagado.
- Combinar la lectura del botón con PWM (`hardware_pwm`) para que un LED se atenúe de manera gradual en vez de conmutar de forma abrupta.