# ADC: Conversión Analógica

Esta práctica introduce el convertidor analógico-digital (ADC) del RP2040, el periférico encargado de traducir una señal de voltaje continua —como la de un sensor de temperatura, un potenciómetro o una fotorresistencia— en un valor numérico que el programa puede procesar. Para esta primera aproximación se emplea el sensor de temperatura integrado en el propio microcontrolador, disponible en uno de los canales del ADC sin requerir ningún componente externo.

## Concepto Teórico

El ADC del RP2040 emplea la técnica de aproximaciones sucesivas (SAR, *Successive Approximation Register*): en cada conversión, un comparador interno contrasta la tensión de entrada contra una tensión de referencia generada internamente, ajustando dicha referencia bit por bit —comenzando por el más significativo— hasta obtener el valor digital que mejor aproxima la tensión analógica original. Este procedimiento se repite por completo en cada conversión y toma un número fijo de ciclos, lo cual determina la velocidad máxima de muestreo alcanzable; dicha velocidad puede reducirse deliberadamente mediante un divisor de reloj configurable, útil cuando conviene promediar más tiempo por lectura a cambio de mayor estabilidad.
 
El periférico dispone de varias entradas posibles —los pines analógicos disponibles más el canal interno de temperatura— pero de un único conversor compartido; por ello, antes de leer un canal es necesario seleccionarlo explícitamente, de manera que un multiplexor interno conecte esa entrada en particular hacia el conversor. La lectura mediante `adc_read()`, empleada en esta práctica, es la forma más simple de operar el periférico: bloquea la ejecución hasta que la conversión concluye. El propio periférico admite formas de operación más avanzadas —recorrido automático de varios canales, muestreo continuo con almacenamiento temporal (FIFO), y generación de interrupciones al completar cada conversión— pensadas para cuando bloquear el programa en cada lectura no resulta conveniente; estas alternativas se documentan en el código como opciones disponibles, aunque no se activan en esta práctica introductoria.

## Hardware y Conexiones

No se requiere ninguna conexión externa: el sensor de temperatura se encuentra integrado en el propio microcontrolador, en un canal del ADC que no está asociado a ningún pin físico.
 
| Elemento | Pin del RP2040 | Descripción |
|---|---|---|
| Sensor de temperatura interno | Canal 4 del ADC (sin pin físico) | Integrado en el silicio del RP2040 |

## Configuración del Proyecto (CMake)

```cmake
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_adc
)
```

## Código Fuente

```c
/**
 * @file main.c
 * @brief Lectura del sensor de temperatura interno mediante el ADC
 *
 * @author obviousfancy
 * @board  pico
 * @sdk    Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    stdio_init_all();

    adc_init();
    adc_set_clkdiv(0);                   // Divisor de reloj: 0 = velocidad maxima de conversion
    adc_set_temp_sensor_enabled(true);    // Habilita el canal interno de temperatura
    adc_select_input(4);                  // Selecciona el canal 4 (sensor interno)
 
    // Si en cambio se empleara un canal externo (0 a 3, asociado a un pin
    // fisico), seria indispensable inicializar antes ese pin para uso
    // analogico, deshabilitando sus resistencias de pull-up/pull-down y
    // su buffer digital:
    // adc_gpio_init(26);  // GPIO26 corresponde al canal 0
 
    // El periferico admite recorrer varios canales de manera automatica
    // (round-robin) en lugar de seleccionarlos uno por uno; no es
    // necesario aqui porque solo se utiliza un canal:
    // adc_set_round_robin(0x1F);  // ejemplo: recorreria los canales 0 a 4
 
    // Las conversiones tambien pueden acumularse en una FIFO interna en
    // lugar de leerse una por una con adc_read(); no es necesario aqui:
    // adc_fifo_setup(true, false, 1, false, false);
 
    // El periferico puede generar una interrupcion al completar cada
    // conversion, en lugar de que el programa consulte el resultado
    // directamente; no es necesario aqui:
    // adc_irq_set_enabled(false);
 
    while (1) {
        uint16_t muestra = adc_read();
        float voltaje = muestra * 3.3f / 4096.0f;
        float temperatura = 27.0f - (voltaje - 0.706f) / 0.001721f;

        printf("Temperatura: %.2f C\n", temperatura);
        sleep_ms(1000);
    }
    return 0;
}
```

## Análisis del Código

`adc_init()` habilita el periférico. `adc_set_temp_sensor_enabled(true)` es indispensable antes de emplear el canal interno de temperatura, ya que dicho canal permanece apagado por defecto para ahorrar energía. `adc_select_input(4)` conecta el canal interno de temperatura al conversor mediante el multiplexor descrito en el Concepto Teórico; de emplearse en cambio uno de los canales asociados a un pin físico (0 a 3), sería indispensable llamar antes a `adc_gpio_init()` sobre ese pin, para deshabilitar sus resistencias de pull-up/pull-down y su buffer digital —condición que el canal interno no necesita, por no estar asociado a ningún pin—.
 
`adc_set_clkdiv()` fija el divisor de reloj que determina la velocidad de conversión; el valor `0` solicita la velocidad máxima disponible, aunque puede incrementarse para espaciar más las conversiones. `adc_set_round_robin(0)` desactiva el recorrido automático entre varios canales —una máscara distinta de cero haría que el periférico alternara solo, sin intervención del programa, entre los canales indicados—, innecesario aquí porque solo se utiliza uno. `adc_fifo_setup()` controla si las conversiones se acumulan en un buffer interno (FIFO) en lugar de leerse una por una; se deja deshabilitada porque esta práctica usa `adc_read()`, una función de conveniencia que realiza una conversión bloqueante sin pasar por la FIFO. `adc_irq_set_enabled(false)` mantiene desactivada la generación de una interrupción al completar cada conversión, ya que el programa consulta el resultado directamente en el ciclo principal en lugar de reaccionar a un evento asíncrono. Estas tres últimas llamadas se dejan explícitas, en su valor deshabilitado, precisamente para mostrar que forman parte de la configuración del periférico aunque esta práctica no las aproveche.
 
`adc_read()` realiza una conversión y retorna el resultado crudo de 12 bits (0 a 4095), que se convierte primero a voltaje y después a temperatura mediante la aproximación lineal que proporciona el fabricante para este sensor.

## Verificación

Ábrase una terminal serial de su preferencia sobre el puerto USB-CDC que expone la placa (por ejemplo, `/dev/ttyACMx` en Linux) a 115200 baudios, en linux puede usarse:

```bash
minicom -b 115200 -D /dev/ttyACM0
```

Debe observarse una lectura de temperatura impresa cada segundo, cercana a la temperatura ambiente, con cierta desviación característica de este sensor no calibrado individualmente en fábrica.

<div align="center">
  <img src="../resources/adcpractice4.png" width="300px" alt="Lectura de temperatura del sensor interno en terminal serial">
  <p><em>Salida esperada en la terminal serial</em></p>
</div>

## Errores Comunes y Variantes

| Síntoma | Causa típica |
|---|---|
| Se obtiene siempre un valor fijo o carente de sentido | No se invocó `adc_set_temp_sensor_enabled(true)` antes de seleccionar el canal 4 |
| No se observa salida alguna en la terminal | `pico_enable_stdio_usb` no fue habilitado con el valor `1` en el CMakeLists |
| Error de *linking* durante la compilación | Ausencia de `hardware_adc` en `target_link_libraries` |

**Variantes:**

- Promediar 10 lecturas consecutivas antes de imprimir el resultado, para reducir el ruido de conversión.
- Modificar `adc_set_clkdiv()` a un valor distinto de `0` y observar el efecto sobre la velocidad con la que cambian las lecturas impresas.
- Encender el LED de la práctica de Blink cuando la temperatura leída supere un umbral definido.
