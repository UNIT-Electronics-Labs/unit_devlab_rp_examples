.syntax unified                     @ Sintaxis unificada: permite escribir ARM y Thumb con la misma notacion de operandos
.cpu cortex-m0plus                  @ Le dice al ensamblador para que nucleo generar codigo: Cortex-M0+ (el que trae el RP2040)
.thumb                              @ El Cortex-M0+ SOLO entiende instrucciones Thumb (16 bits), no tiene modo ARM de 32 bits

@ --- Direcciones (ver RP2040 Datasheet, seccion 2.2 Address Map) ---
@ Cada periferico vive en un rango fijo de memoria; escribir/leer esas
@ direcciones ES la forma de "hablarle" al hardware (memory-mapped I/O).

.equ IO_BANK0_GPIO25_CTRL, 0x400140cc  @ IO_BANK0_BASE + 0x004 + 25*8
                                        @ Registro CTRL del pin 25: aqui se elige que "funcion" cumple el pin (UART, PWM, SIO, etc.)
.equ SIO_GPIO_OE_SET,      0xd0000024  @ SIO_BASE + 0x024
                                        @ Registro de "Output Enable SET": poner un bit en 1 aqui habilita ese GPIO como salida,
                                        @ y es atomico (no toca los demas bits, a diferencia de leer+modificar+escribir GPIO_OE)
.equ SIO_GPIO_OUT_XOR,     0xd000001c  @ SIO_BASE + 0x01c
                                        @ Alias "XOR" del registro OUT: escribir 1 en un bit aqui INVIERTE ese pin (toggle atomico),
                                        @ evitando tener que leer el estado actual antes de cambiarlo

.equ GPIO25_BIT,           0x02000000  @ 1 << 25 -> mascara de un solo bit para referirnos al pin 25 dentro de un registro de 32 bits
.equ FUNCSEL_SIO,          5           @ F5 = SIO (ver tabla de funciones GPIO). SIO = Single-cycle I/O, control directo por software

.section .text                      @ A partir de aqui, el codigo va a la seccion .text (memoria de programa / flash)
.global main                        @ Hace visible el simbolo "main" fuera de este archivo, para que el linker lo use como entrada
.thumb_func                         @ Marca la etiqueta siguiente como funcion Thumb: el linker pondra el bit 0 de su direccion en 1
main:
    @ 1. Seleccionar funcion SIO en GPIO25_CTRL (FUNCSEL)
    ldr   r0, =IO_BANK0_GPIO25_CTRL @ r0 = direccion del registro CTRL de GPIO25 (se toma del "literal pool", ldr con "=" carga constantes de 32 bits)
    movs  r1, #FUNCSEL_SIO          @ r1 = 5 (codigo de funcion SIO). movs cabe en 16 bits porque el valor es un inmediato pequeno
    str   r1, [r0]                  @ Escribe r1 en la direccion de r0: a partir de ahora GPIO25 esta controlado por el software (SIO)

    @ 2. Habilitar GPIO25 como salida via SIO
    ldr   r0, =SIO_GPIO_OE_SET      @ r0 = direccion del registro OE_SET (habilitar salidas)
    ldr   r1, =GPIO25_BIT           @ r1 = mascara con el bit 25 encendido (0x02000000)
    str   r1, [r0]                  @ Al escribir esta mascara en OE_SET, SOLO el bit 25 pasa a "salida"; el resto de pines no se altera

loop:
    @ 3. Alternar el estado del pin (toggle atomico)
    ldr   r0, =SIO_GPIO_OUT_XOR     @ r0 = direccion del alias XOR del registro OUT
    ldr   r1, =GPIO25_BIT           @ r1 = mascara del bit 25 (se recarga porque este bucle se repite para siempre)
    str   r1, [r0]                  @ Escribir la mascara aqui hace XOR con el estado actual del pin: si estaba en 0 pasa a 1 y viceversa

    @ 4. Retardo por software (ajustar segun clk_sys)
    ldr   r2, = 40000000            @ r2 = numero de iteraciones del bucle de espera; a mas alto, mas tiempo dura el retardo
delay:
    subs  r2, r2, #1                @ r2 = r2 - 1, y ademas actualiza los flags (la "s" final): Z se activa cuando r2 llega a 0
    bne   delay                     @ "Branch if Not Equal": si Z sigue en 0 (r2 != 0), vuelve a "delay" -> esto es la espera activa

    b     loop                      @ Salto incondicional al inicio del bucle principal: vuelve a invertir el pin y repite para siempre

.end                                 @ Indica al ensamblador que aqui termina el archivo fuente
