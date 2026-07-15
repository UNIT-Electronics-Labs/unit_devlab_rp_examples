.syntax unified                     @ Sintaxis unificada: mismo formato de operandos para instrucciones ARM y Thumb
.cpu cortex-m0plus                  @ Codigo generado para el nucleo Cortex-M0+ del RP2040
.thumb                               @ Modo Thumb (16 bits): el unico que entiende el Cortex-M0+

.section .rodata                    @ Datos de solo lectura (van a flash, no a RAM)
mensaje:                            @ Etiqueta que apunta al inicio de la cadena
    .asciz "Hello World desde USB\n" @ Cadena terminada en NUL (\0), lista para pasarsela a printf

.section .text                      @ A partir de aqui el codigo va a la seccion .text (memoria de programa/flash)
.global main                        @ Simbolo visible para el linker: sera el punto de entrada del programa
.thumb_func                         @ Le indica al ensamblador/linker que "main" es una funcion en modo Thumb
main:
    @ Inicializa stdio sobre USB (enumeracion CDC la resuelve
    @ el stack TinyUSB integrado en el SDK; no es reproducible
    @ en ensamblador puro)
    bl    stdio_init_all             @ "Branch with Link": guarda la direccion de retorno en LR y salta a stdio_init_all

    @ Pequena espera para permitir que el host termine de
    @ enumerar el dispositivo USB antes del primer printf
    ldr   r0, =2000                  @ r0 = primer argumento de sleep_ms (2000 ms), tomado del literal pool
    bl    sleep_ms                   @ Espera bloqueante de 2 segundos antes de intentar imprimir

bucle:                                @ Etiqueta de destino para el salto incondicional del final (bucle infinito)
    ldr   r0, =mensaje                @ r0 = direccion de la cadena "mensaje" (primer argumento de printf)
    bl    printf                      @ Llama a printf del SDK; el mensaje sale por el puerto serie USB (CDC)

    ldr   r0, =1000                   @ r0 = primer argumento de sleep_ms (1000 ms)
    bl    sleep_ms                    @ Espera 1 segundo entre cada mensaje impreso

    b     bucle                       @ Salto incondicional: vuelve a imprimir el mensaje y repite para siempre

.end                                  @ Indica al ensamblador que aqui termina el archivo fuente