// ─────────────────────────────────────────────
// sdk_inlines.c
// @author  obviousfancy
// @board   pico
// @sdk     Raspberry Pi Pico SDK 2.2.0
// ─────────────────────────────────────────────
//
// gpio_set_dir y gpio_put estan declaradas como "static inline" en el
// header hardware/gpio.h del Pico SDK. Una funcion inline no genera un
// simbolo de enlazado (linker symbol): el compilador de C inserta el
// codigo directamente en el sitio de la llamada, pero un archivo de
// ensamblador puro (.S) no tiene forma de "insertar" ese codigo, solo
// puede saltar (BL) a una direccion con nombre. Por eso, main.S falla
// con "undefined reference" al intentar usarlas directamente.
//
// La solucion es este archivo auxiliar en C: cada wrapper de abajo
// SI es una funcion real (no inline), por lo que el compilador genera
// un simbolo enlazable con ese nombre. Internamente, el wrapper solo
// invoca la version inline del SDK, que se expande normalmente porque
// aqui si estamos en un contexto de compilacion C.
//
// gpio_init no requiere wrapper porque ya es una funcion real (no
// inline) definida en gpio.c dentro del propio SDK.

#include "hardware/gpio.h"

void asm_gpio_set_dir(uint gpio, bool out) {
    gpio_set_dir(gpio, out);
}

void asm_gpio_put(uint gpio, bool value) {
    gpio_put(gpio, value);
}