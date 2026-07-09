# Ejemplo 00: LED Blinking

Este ejemplo no corresponde a una práctica formal del temario; su propósito es servir como verificación inicial del entorno de desarrollo. Antes de introducir periféricos adicionales, es necesario confirmar que la cadena de herramientas —SDK, compilador y CMake— se encuentra correctamente configurada. Una vez que este proyecto compila y se carga en la placa sin inconvenientes, los ejemplos subsecuentes únicamente incorporan periféricos sobre esta misma base.

## Conexiones

No se requiere conexión externa. Se utiliza el LED integrado en la placa.

| Señal | Pin |
|---|---|
| LED integrado | GPIO 25 |

## Estructura del proyecto

```
blink/
├── build
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── blink.c
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(parpadeo_led C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(parpadeo_led
    main.c
)

target_link_libraries(parpadeo_led
    pico_stdlib
)

pico_add_extra_outputs(parpadeo_led)
```

No enlaza ninguna librería `hardware_*` de forma nativa, porque `pico_stdlib` ya contiene GPIO básico y `sleep_ms`, pero si queremos tener de manera verbose 
las funcionalidades utilizadas podemos agregar hardware/gpio.h

## main.c

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN 25


int main(){

    gpio_init(LED_PIN); 
    gpio_set_dir(LED_PIN, GPIO_OUT); 
    
    while(1){
        gpio_put(LED_PIN, 1); // Turn on LED
        sleep_ms(1000); // Delay for 1 second
        gpio_put(LED_PIN, 0); // Turn off LED
        sleep_ms(1000); // Delay for 1 second
    }
    return 0;
}
```

## Explicación

Tres llamadas hacen todo el trabajo: `gpio_init` reserva el pin y lo inicializa como entrada o salida digital, `gpio_set_dir` lo configura como salida, y el ciclo alterna el voltaje con `gpio_put` cada 500 ms. El detalle de cada función está en la [guía de C, sección GPIO](../../guide/c-programming.md#gpio-pico_stdlib).

## Compilar y cargar

### Opcion A: Automatico 
Podemos flashear usando el CH552, 
```bash
pico-flash
```

### Opcion B: Flash Manual
La segunda forma de flashearlo es usando la forma tradicional de subir los archivos, a nuestro RP2040, siguiendo explicitamente los siguientes pasos: 
1. Presiona el boton BOOTSEL
2. Con el boton BOOTSEL presionado, conectamos el RP2040 a nuestro computador, y nos aparecera el RP2040 como una unidad extraible
3. Copiamos el archivo `blink.uf2` a la placa con el modo BOOTSEL aun activo, y el programa empezara a correr en nuestro microcontrolador.

## Verificación

El LED integrado debe parpadear a intervalos de medio segundo. Si no se observa actividad, se recomienda verificar en primer lugar que el archivo .uf2 se haya transferido correctamente —la unidad de almacenamiento se desmonta automáticamente al finalizar la copia— antes de revisar el código fuente, o si no se desmonto, presionar el boton RST.

## Variantes

- Cambiar el patrón a un parpadeo tipo "SOS" (Morse).
- Controlar la frecuencia con un GPIO de entrada (ver el [ejemplo 01](./gpio.md)).
- Agregar un segundo LED externo en otro pin y alternar entre ambos.

## Errores comunes

| Síntoma | Causa probable |
|---|---|
| Falla la compilación: `PICO_BOARD` no reconocido | El archivo `pico_sdk_import.cmake` no fue copiado al directorio del proyecto |
| Compila correctamente pero no se genera el `.uf2` | Ausencia de la instrucción `pico_add_extra_outputs()` en el `CMakeLists.txt` |
| El LED no enciende en ningún momento | La placa no ingresó al modo BOOTSEL antes de realizar la copia del archivo |
