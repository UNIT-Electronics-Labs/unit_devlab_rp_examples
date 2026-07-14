# IRQ: Interrupciones Externas

La práctica anterior (GPIO) detectó el estado de un botón mediante muestreo periódico: el núcleo consulta el pin de manera constante, incluso cuando no ocurre ningún cambio. Esta práctica aborda la alternativa basada en interrupciones, en la que el núcleo permanece libre para otras tareas y solo es desviado hacia una función específica cuando ocurre el evento de interés. Este mecanismo es indispensable en sistemas con restricciones de energía o con múltiples tareas concurrentes, donde consultar constantemente cada entrada resultaría impráctico.

## Concepto Teórico

A diferencia del muestreo periódico, una interrupción permite que el procesador continúe ejecutando su tarea principal y sea desviado, de manera automática, hacia una función especial —la rutina de servicio de interrupción, o ISR— únicamente cuando ocurre un evento específico, sin necesidad de consultarlo de manera constante. En el RP2040, la detección de flancos (de subida, de bajada, o ambos) y de niveles se resuelve de manera independiente para cada núcleo del procesador; la señal resultante llega al núcleo Cortex-M0+ a través de su controlador de interrupciones anidadas y vectorizadas (NVIC), el cual decide, entre otras cosas, la prioridad de atención cuando ocurren varias interrupciones de manera simultánea. El SDK permite ajustar dicha prioridad mediante `irq_set_priority()`, aunque esta práctica utiliza la prioridad por defecto, suficiente cuando solo hay una fuente de interrupción activa.
 
El SDK expone esta lógica mediante una única función de registro de retrollamada (*callback*) compartida para todas las interrupciones GPIO de un núcleo: `gpio_set_irq_enabled_with_callback()`. Esto tiene una consecuencia importante: si la aplicación necesita habilitar interrupciones sobre más de un pin, únicamente la primera llamada debe emplear esta función; las llamadas subsecuentes deben usar `gpio_set_irq_enabled()`, ya que una segunda llamada a la versión con *callback* sobrescribiría la función de retrollamada previamente registrada.

## Hardware y Conexiones

| Elemento | Pin del RP2040 | Descripción |
|---|---|---|
| Botón de panel | GPIO24 | Mismo botón empleado en la práctica de GPIO; un extremo al pin, el otro a GND. Se utiliza el pull-up interno |

## Configuración del Proyecto (CMake)

```cmake
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_gpio
)
```

## Código Fuente

```c
/**
 * @file main.c
 * @brief Deteccion de flancos en un boton mediante interrupcion externa
 *
 * @author obviousfancy
 * @board  pico
 * @sdk    Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* ─── Defines ──────────────────────────────────────────── */
#define BUTTON_PIN 24

/* ─── Rutina de servicio de interrupcion (ISR) ────────────
 * Se ejecuta automaticamente cuando ocurre alguno de los
 * eventos configurados sobre el pin indicado.
 */
void boton_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        printf("Boton presionado (interrupcion en GPIO %d)\n", gpio);
    } else if (events & GPIO_IRQ_EDGE_RISE) {
        printf("Boton liberado (interrupcion en GPIO %d)\n", gpio);
    }
}

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // La entrada admite histeresis (disparador Schmitt) para filtrar ruido
    // electrico alrededor del umbral de conmutacion; ya viene habilitada
    // por defecto, por lo que no es necesario activarla explicitamente:
    // gpio_set_input_hysteresis_enabled(BUTTON_PIN, true);
 
    gpio_set_irq_enabled_with_callback(
        BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true,
        &boton_callback
    );

    // La prioridad de esta interrupcion frente a otras fuentes activas
    // puede ajustarse; no es necesario aqui porque el boton es la unica
    // fuente de interrupcion en uso. Ejemplo de uso:
    // irq_set_priority(IO_IRQ_BANK0, PICO_HIGHEST_IRQ_PRIORITY);

    while (1) {
        // El nucleo queda libre para otras tareas; la deteccion
        // del boton ocurre de forma asincrona, por interrupcion.
        tight_loop_contents();
    }
}
```

## Análisis del Código

La inicialización del pin (`gpio_init`, `gpio_set_dir`, `gpio_pull_up`) es idéntica a la de la práctica de GPIO, ya que se trata del mismo botón; la línea comentada de histéresis tampoco es necesaria aquí, por la misma razón señalada en esa práctica. Lo nuevo es la configuración de la interrupción: `gpio_set_irq_enabled_with_callback()` configura el pin para generar una interrupción ante los eventos indicados —flanco de subida (`GPIO_IRQ_EDGE_RISE`) y flanco de bajada (`GPIO_IRQ_EDGE_FALL`)— y registra `boton_callback` como la función que el núcleo ejecutará automáticamente cada vez que ocurra alguno de ellos. La retrollamada recibe como argumentos el pin que generó la interrupción (`gpio`) y una máscara con el evento detectado (`events`), lo que permite distinguir, dentro de una misma función, si el botón fue presionado o liberado. La línea comentada sobre `irq_set_priority()` documenta que la prioridad de esta interrupción es ajustable frente a otras fuentes activas; no resulta necesaria aquí porque el botón es la única fuente de interrupción en uso. El ciclo principal queda, en consecuencia, prácticamente vacío: `tight_loop_contents()` es una instrucción del SDK que evita que el compilador optimice de manera indebida un ciclo sin cuerpo, y refleja el hecho de que el núcleo permanece disponible para otras tareas mientras la detección del botón ocurre de manera asíncrona.
 
A diferencia de la práctica de GPIO, este código no incorpora un criterio de antirrebote: el rebote mecánico del contacto puede generar varias interrupciones adicionales durante una sola pulsación, lo cual se deja como punto de partida para experimentación.

## Verificación

Ábrase una terminal serial de su preferencia sobre el puerto USB-CDC que expone la placa (por ejemplo, `/dev/ttyACM0` en Linux) a 115200 baudios, en linux puede usarse:

```bash
minicom -b 115200 -D /dev/ttyACM0
```

Cada vez que se presione o se libere el botón debe imprimirse de inmediato la línea correspondiente (`Boton presionado` o `Boton liberado`), sin el retardo de 20 ms perceptible en la práctica de GPIO.

<div align="center">
  <img src="../resources/03irq.png" width="300px" alt="Mensajes Boton presionado y Boton liberado generados por interrupcion en terminal serial">
  <p><em>Salida esperada en la terminal serial</em></p>
</div>

## Errores Comunes y Variantes

| Síntoma | Causa típica |
|---|---|
| No se genera ninguna interrupción | Falta `gpio_pull_up()` (el pin queda flotante y genera eventos espurios), o el tipo de evento configurado no corresponde al flanco esperado |
| Se registran varios mensajes por cada pulsación | Rebote mecánico del contacto; este código no incorpora antirrebote |
| Al agregar una segunda interrupción en otro pin, la primera deja de funcionar | Se usó `gpio_set_irq_enabled_with_callback()` en la segunda llamada, sobrescribiendo el callback ya registrado; debe usarse `gpio_set_irq_enabled()` |
 
**Variantes:**
 
- Incorporar un criterio de antirrebote dentro de la propia rutina de interrupción, comparando la marca de tiempo (`time_us_64()`) del evento actual contra la del anterior y descartando eventos demasiado cercanos entre sí.
- Habilitar una segunda interrupción sobre un pin adicional, empleando `gpio_set_irq_enabled()` para no sobrescribir el callback ya registrado.
- Contar el número de interrupciones atendidas y mostrar el total cada cierto número de eventos, en lugar de imprimir en cada uno.
