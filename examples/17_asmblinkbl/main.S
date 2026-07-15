// ─────────────────────────────────────────────
// Practica: Blink en Ensamblador ARM Thumb (BL)
// @author  obviousfancy
// @board   pico
// @sdk     Raspberry Pi Pico SDK 2.2.0
// ─────────────────────────────────────────────
//
// Esta practica reescribe Blink (Practica 1 de Perifericos Basicos)
// a nivel de ensamblador ARM Thumb puro. En lugar de acceder a los
// registros de silicio de forma directa, se utiliza la instruccion
// BL (Branch with Link) para invocar las funciones del Pico C/C++ SDK.
// Esto permite observar el mecanismo de llamada a subrutina (guardado
// de direccion de retorno en LR) sin renunciar a la seguridad y
// portabilidad de las funciones oficiales del SDK.
//
// Nota tecnica: gpio_set_dir y gpio_put estan declaradas como "static
// inline" en el SDK, por lo que no generan un simbolo de enlazado
// alcanzable desde ensamblador puro. Para resolver esto, se acompana
// este archivo con sdk_inlines.c, que expone dos funciones reales
// (asm_gpio_set_dir, asm_gpio_put) que envuelven a las version inline.
// gpio_init y sleep_ms si son funciones reales del SDK y se invocan
// de forma directa.

.syntax unified                     // Sintaxis unificada: mismo formato de operandos para instrucciones ARM y Thumb
.cpu cortex-m0plus                  // Codigo generado para el nucleo Cortex-M0+ del RP2040
.thumb                               // Modo Thumb (16 bits): el unico que entiende el Cortex-M0+

// ─────────────────────────────────────────────
// Seccion de datos: constantes
// ─────────────────────────────────────────────
.equ LED_PIN,     25      // GPIO25, LED integrado de la Pico
.equ GPIO_OUT,     1      // Valor de direccion: salida
.equ DELAY_MS,   500      // Medio segundo de espera por estado

// ─────────────────────────────────────────────
// Seccion de codigo
// ─────────────────────────────────────────────
.section .text                      // A partir de aqui el codigo va a la seccion .text (memoria de programa/flash)

.global main                        // Simbolo visible para el linker: sera el punto de entrada del programa
.thumb_func                         // Le indica al ensamblador/linker que "main" es una funcion en modo Thumb
main:
    // gpio_init(LED_PIN)
    movs   r0, #LED_PIN             // r0 = primer argumento de la funcion (convencion AAPCS: r0-r3 para parametros)
    bl     gpio_init                // "Branch with Link": guarda la direccion de retorno en LR y salta a gpio_init

    // asm_gpio_set_dir(LED_PIN, GPIO_OUT) -- wrapper de gpio_set_dir (inline)
    movs   r0, #LED_PIN             // r0 = primer argumento (pin)
    movs   r1, #GPIO_OUT            // r1 = segundo argumento (direccion: 1 = salida)
    bl     asm_gpio_set_dir         // Llama al wrapper en C que expone la funcion inline del SDK

loop:
    // asm_gpio_put(LED_PIN, 1) -- wrapper de gpio_put (inline)
    movs   r0, #LED_PIN             // r0 = pin a modificar
    movs   r1, #1                   // r1 = valor a escribir (1 = encendido)
    bl     asm_gpio_put             // Llama al wrapper; LR se sobreescribe con la direccion de retorno aqui

    // sleep_ms(DELAY_MS)
    ldr    r0, =DELAY_MS            // r0 = 500 (constante de 32 bits, se toma del literal pool)
    bl     sleep_ms                 // Espera bloqueante de 500 ms con el LED encendido

    // asm_gpio_put(LED_PIN, 0) -- wrapper de gpio_put (inline)
    movs   r0, #LED_PIN             // r0 = pin a modificar
    movs   r1, #0                   // r1 = valor a escribir (0 = apagado)
    bl     asm_gpio_put             // Apaga el LED

    // sleep_ms(DELAY_MS)
    ldr    r0, =DELAY_MS            // r0 = 500 ms nuevamente, con el LED apagado
    bl     sleep_ms                 // Espera bloqueante de 500 ms con el LED apagado

    b      loop                     // Salto incondicional: vuelve a encender el LED y repite para siempre

.end