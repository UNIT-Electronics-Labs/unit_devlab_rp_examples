# Ensamblador: Hello World por USB

Esta práctica introduce la capa de ensamblador ARM Thumb del curso mediante el ejemplo más simple posible: enviar una cadena de texto por el puerto USB integrado del RP2040. A diferencia de las prácticas de la sección "Periféricos Básicos", aquí el código en `main.S` se reduce a un arranque que invoca, mediante instrucciones de ensamblador explícitas, a funciones ya compiladas del Pico C/C++ SDK. El objetivo no es reemplazar al SDK, sino mostrar de forma directa cómo una llamada de alto nivel como `printf` se traduce, en última instancia, en una secuencia de instrucciones Thumb con paso de parámetros por registros (`r0`, `r1`, ...) y una instruccion de rama con enlace (`bl`).

## Concepto Teórico

El procesador del RP2040 es un ARM Cortex-M0+, que implementa el conjunto de instrucciones **ARMv6-M Thumb**. Este conjunto está optimizado para densidad de código: la mayoría de las instrucciones ocupan 16 bits, con un subconjunto reducido de instrucciones de 32 bits (entre ellas, `BL`, usada para llamadas a subrutina).

Dos elementos son centrales para entender el código de esta práctica:

- **Convención de paso de parámetros (AAPCS):** al llamar a una función, los primeros argumentos se colocan en los registros `r0`, `r1`, `r2` y `r3`, en ese orden. El valor de retorno, si existe, queda en `r0`. Esta convención es la que permite que una instrucción en ensamblador invoque directamente una función escrita en C, siempre que se respete el orden y el tipo de los argumentos.
- **Instrucción `BL` (Branch with Link):** salta a la dirección de una subrutina y guarda la dirección de retorno en el registro de enlace (`LR`), permitiendo que la subrutina, al terminar, regrese al punto exacto desde donde fue llamada.

En el caso particular del USB, la inicialización y comunicación por este periférico requieren un stack de protocolo completo (enumeración del dispositivo, descriptores USB, gestión de endpoints según la clase CDC-ACM). Esta lógica está implementada en TinyUSB, integrado en el Pico C/C++ SDK, y no resulta práctico ni pedagógicamente razonable reproducirla en ensamblador puro. Por esta razón, el ensamblador de esta práctica se limita a invocar `stdio_init_all()` y `printf()`, ya provistas por el SDK, en lugar de manipular registros de silicio directamente como ocurre en las prácticas de GPIO o PWM. Esta decisión ilustra, precisamente, el propósito de la capa de ensamblador dentro del curso: mostrar el mecanismo de llamada de bajo nivel que subyace a cualquier función del SDK, sin necesidad de reimplementar el USB completo.

## Hardware y Conexiones

Esta práctica no requiere conexiones externas. La comunicación se realiza mediante el puerto **USB-C integrado** de la placa RP2040.

| Conexión | Descripción |
|---|---|
| USB-C (placa RP2040) | Conexión directa a la PC; expone un puerto serial virtual (CDC-ACM) |

## Configuración del Proyecto — CMake

```cmake
pico_enable_stdio_usb(18_uarthelloworld 1)
pico_enable_stdio_uart(18_uarthelloworld 0)

target_link_libraries(18_uarthelloworld
    pico_stdlib
)
```

La habilitación de `pico_enable_stdio_usb` en 1 es la que enruta `printf` hacia el USB en tiempo de enlace; `pico_enable_stdio_uart` se deshabilita explícitamente en 0 para evitar que el mismo flujo de `stdio` se duplique hacia la UART física.

## Código Fuente — `main.c`

```asm
/**
 * @file   main.c
 * @author obviousfancy
 * @board  pico
 * @sdk    Raspberry Pi Pico SDK 2.2.0
 */

.syntax unified
.cpu cortex-m0plus
.thumb

.section .rodata
mensaje:
    .asciz "Hello World desde USB\n"

.section .text
.global main
.thumb_func
main:
    @ Inicializa stdio sobre USB. La enumeracion CDC ante el
    @ host la resuelve el stack TinyUSB integrado en el SDK;
    @ no es reproducible en ensamblador puro.
    bl    stdio_init_all

    @ Espera para permitir que el host termine de enumerar
    @ el dispositivo USB antes del primer printf. Sin esta
    @ espera, los primeros mensajes se pierden.
    ldr   r0, =2000
    bl    sleep_ms

bucle:
    ldr   r0, =mensaje
    bl    printf

    ldr   r0, =1000
    bl    sleep_ms

    b     bucle

.end
```

## Análisis del Código

- **`.syntax unified` / `.cpu cortex-m0plus` / `.thumb`:** directivas del ensamblador GNU que fijan la sintaxis unificada de ARM y el juego de instrucciones Thumb del Cortex-M0+, sin las cuales el ensamblador no podría validar ni codificar correctamente las instrucciones siguientes.
- **`.section .rodata` / `mensaje: .asciz ...`:** reserva la cadena en la sección de solo lectura, terminada en NUL (`.asciz`), tal como espera `printf`.
- **`.global main` / `.thumb_func`:** exponen el símbolo `main` para que el enlazador lo ubique como punto de entrada del programa, y marcan explícitamente que se trata de una función en modo Thumb (necesario para que el bit menos significativo de la dirección se fije en 1, como exige la arquitectura al saltar entre modos).
- **`bl stdio_init_all`:** primera llamada real a una función de C desde ensamblador. No requiere cargar ningún argumento en `r0`, ya que la función no recibe parámetros.
- **`ldr r0, =2000` / `bl sleep_ms`:** ejemplo de paso de un único argumento por `r0` (el valor en milisegundos), ilustrando la convención AAPCS descrita en el Concepto Teórico.
- **`ldr r0, =mensaje` / `bl printf`:** se carga en `r0` la dirección de la cadena (no su contenido) y se invoca `printf`, que internamente reconoce que su primer argumento es un puntero a cadena de formato.
- **`bucle:` / `b bucle`:** a diferencia de un `while(1)` en C, aquí el bucle infinito se expresa como una instrucción de salto incondicional (`B`) que regresa a la etiqueta `bucle`, repitiendo indefinidamente el envío del mensaje cada segundo.

## Verificación

Una vez flasheado el programa mediante `pico-flash` (SWD), se debe abrir una terminal serial (por ejemplo `pico-monitor.py` o cualquier monitor de puerto serie) apuntando al puerto virtual USB-CDC que expone la placa. Tras la espera inicial de 2 segundos, debe observarse el siguiente mensaje repitiéndose cada segundo:

```
Hello World desde USB
Hello World desde USB
Hello World desde USB
```


## Errores Comunes y Variantes

| Error común | Causa | Solución |
|---|---|---|
| No aparece ningún mensaje en la terminal | `pico_enable_stdio_usb` no está en 1, o falta el `target_link_libraries` con `pico_stdlib` | Verificar el `CMakeLists.txt` del proyecto |
| Se pierden los primeros mensajes | El host no terminó de enumerar el dispositivo USB antes del primer `printf` | Aumentar el retardo inicial (`sleep_ms`) tras `stdio_init_all` |
| El programa no arranca / queda en `fault` | Falta la directiva `.thumb_func` sobre `main`, y el enlazador no fija correctamente el bit Thumb de la dirección | Confirmar la presencia de `.thumb_func` inmediatamente antes de la etiqueta |
| Caracteres corruptos o líneas cortadas en la terminal | Configuración de fin de línea del monitor serial no coincide con `\n` | Configurar el monitor en modo LF o CRLF según corresponda |

**Variantes propuestas:**

1. Modificar el mensaje para que incluya un contador que se incremente en cada iteración del bucle, pasando el valor como segundo argumento de `printf` (`r1`).
2. Reemplazar el retardo fijo de 1000 ms por un valor cargado desde una constante `.equ`, y experimentar con distintos intervalos.
3. Agregar una segunda cadena en `.rodata` y alternar entre ambas en cada iteración del bucle, usando un registro como bandera de selección.
