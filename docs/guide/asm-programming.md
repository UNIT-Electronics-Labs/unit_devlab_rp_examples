# Programación en Ensamblador para RP2040

Esta guía introduce la programación en ensamblador ARM Thumb-2 para el RP2040 (Cortex-M0+).

## Arquitectura Cortex-M0+

El RP2040 contiene dos núcleos ARM Cortex-M0+ que ejecutan el conjunto de instrucciones **Thumb-2**, un subconjunto de ARMv6-M optimizado para tamaño de código y eficiencia energética.

### Registros

- **R0-R7**: Registros de propósito general (acceso directo)
- **R8-R12**: Registros adicionales (acceso limitado)
- **R13 (SP)**: Stack Pointer
- **R14 (LR)**: Link Register (dirección de retorno)
- **R15 (PC)**: Program Counter
- **APSR**: Flags de estado (N, Z, C, V)

## Sintaxis Básica

### Archivo .S

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.section .text

.global mi_funcion
.type mi_funcion, %function

mi_funcion:
    // Código aquí
    movs r0, #42    // Mover 42 a R0
    bx lr           // Retornar
```

### Directivas

- `.syntax unified`: Usar sintaxis ARM unificada
- `.cpu cortex-m0plus`: Target específico
- `.thumb`: Modo Thumb (16/32-bit)
- `.global`: Hacer símbolo visible externamente
- `.type`: Declarar tipo de símbolo
- `.section`: Definir sección de memoria

## Instrucciones Básicas

### Movimiento de Datos

```asm
movs r0, #100       // R0 = 100 (inmediato)
movs r1, r0         // R1 = R0 (registro)
ldr r2, =0x40000000 // R2 = dirección (literal pool)
```

### Aritmética

```asm
adds r0, r1, r2     // R0 = R1 + R2
subs r0, r1, #5     // R0 = R1 - 5
muls r0, r1, r0     // R0 = R0 * R1
```

### Lógica

```asm
ands r0, r1         // R0 = R0 AND R1
orrs r0, r1         // R0 = R0 OR R1
eors r0, r1         // R0 = R0 XOR R1
mvns r0, r1         // R0 = NOT R1
```

### Desplazamientos

```asm
lsls r0, r1, #2     // R0 = R1 << 2 (lógico)
lsrs r0, r1, #3     // R0 = R1 >> 3 (lógico)
asrs r0, r1, #1     // R0 = R1 >> 1 (aritmético)
```

### Memoria

```asm
ldr r0, [r1]        // R0 = *R1 (32-bit)
ldrh r0, [r1]       // R0 = *R1 (16-bit)
ldrb r0, [r1]       // R0 = *R1 (8-bit)
str r0, [r1]        // *R1 = R0 (32-bit)
strh r0, [r1]       // *R1 = R0 (16-bit)
strb r0, [r1]       // *R1 = R0 (8-bit)

ldr r0, [r1, #4]    // R0 = *(R1 + 4)
str r0, [r1, r2]    // *(R1 + R2) = R0
```

### Control de Flujo

```asm
b etiqueta          // Salto incondicional
beq etiqueta        // Saltar si igual (Z=1)
bne etiqueta        // Saltar si no igual (Z=0)
blt etiqueta        // Saltar si menor
bge etiqueta        // Saltar si mayor o igual
bl funcion          // Llamar a función (guarda LR)
bx lr               // Retornar de función
```

### Comparación

```asm
cmp r0, r1          // Comparar R0 con R1 (actualiza flags)
cmp r0, #10         // Comparar R0 con 10
tst r0, r1          // Test bits (AND sin guardar)
```

## Ejemplo: Blink LED

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

// Direcciones de registros GPIO
.equ SIO_BASE,      0xd0000000
.equ GPIO_OUT_SET,  0x014
.equ GPIO_OUT_CLR,  0x018
.equ LED_PIN,       25

.section .text
.global blink_led
.type blink_led, %function

blink_led:
    push {r4, lr}
    
    // R4 = SIO_BASE
    ldr r4, =SIO_BASE
    
    // R1 = máscara del LED (1 << 25)
    movs r1, #1
    lsls r1, r1, #LED_PIN
    
loop:
    // Encender LED
    str r1, [r4, #GPIO_OUT_SET]
    
    // Delay
    ldr r0, =500000
    bl delay_cycles
    
    // Apagar LED
    str r1, [r4, #GPIO_OUT_CLR]
    
    // Delay
    ldr r0, =500000
    bl delay_cycles
    
    b loop

// Función delay simple
delay_cycles:
    subs r0, #1
    bne delay_cycles
    bx lr

.size blink_led, . - blink_led
```

## Ejemplo: Suma de Array

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.section .text
.global suma_array
.type suma_array, %function

// uint32_t suma_array(uint32_t *arr, uint32_t len)
// R0 = puntero al array
// R1 = longitud
// Retorna suma en R0

suma_array:
    movs r2, #0         // R2 = suma (acumulador)
    movs r3, #0         // R3 = índice
    
suma_loop:
    cmp r3, r1          // Comparar índice con longitud
    bge suma_done       // Si índice >= len, terminar
    
    ldr r4, [r0, r3]    // R4 = arr[índice]
    adds r2, r2, r4     // suma += arr[índice]
    adds r3, #4         // índice += 4 (bytes)
    b suma_loop
    
suma_done:
    movs r0, r2         // Retornar suma en R0
    bx lr

.size suma_array, . - suma_array
```

## Integración con C

### Llamar ASM desde C

```c
// Declaración en C
extern uint32_t suma_array(uint32_t *arr, uint32_t len);

int main() {
    uint32_t datos[] = {10, 20, 30, 40};
    uint32_t resultado = suma_array(datos, 4);
    // resultado = 100
}
```

### CMakeLists.txt

```cmake
add_executable(mi_programa
    main.c
    funciones.S
)

target_link_libraries(mi_programa
    pico_stdlib
    hardware_gpio
)

pico_add_extra_outputs(mi_programa)
```

### Inline Assembly en C

```c
static inline void enable_interrupts(void) {
    __asm__ volatile ("cpsie i" : : : "memory");
}

static inline void disable_interrupts(void) {
    __asm__ volatile ("cpsid i" : : : "memory");
}

static inline uint32_t get_sp(void) {
    uint32_t result;
    __asm__ volatile ("mov %0, sp" : "=r" (result));
    return result;
}
```

## Acceso a Periféricos

### GPIO

```asm
// Configurar pin 25 como salida
.equ IOBANK0_BASE,  0x40014000
.equ GPIO25_CTRL,   0x0cc

ldr r0, =IOBANK0_BASE
movs r1, #5             // Función SIO
str r1, [r0, #GPIO25_CTRL]

// Escribir en GPIO
.equ SIO_BASE,      0xd0000000
.equ GPIO_OE_SET,   0x024
.equ GPIO_OUT_SET,  0x014

ldr r0, =SIO_BASE
movs r1, #1
lsls r1, #25            // Máscara para pin 25

str r1, [r0, #GPIO_OE_SET]   // Habilitar salida
str r1, [r0, #GPIO_OUT_SET]  // Poner en HIGH
```

## Convención de Llamadas AAPCS

El **ARM Architecture Procedure Call Standard** define cómo pasar parámetros:

- **R0-R3**: Primeros 4 argumentos
- **R0**: Valor de retorno
- **R4-R11**: Preservados por la función llamada (callee-saved)
- **R12**: Scratch register
- **LR (R14)**: Dirección de retorno

```asm
mi_funcion:
    push {r4-r7, lr}    // Guardar registros preservados
    
    // Usar R0-R3 como parámetros
    // Usar R4-R7 libremente (restaurar al salir)
    
    pop {r4-r7, pc}     // Restaurar y retornar
```

## Recursos

- [ARMv6-M Architecture Reference Manual](https://developer.arm.com/documentation/ddi0419/latest/)
- [Cortex-M0+ Generic User Guide](https://developer.arm.com/documentation/dui0662/latest/)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf) - Capítulo 2.1
- [GNU Assembler Manual](https://sourceware.org/binutils/docs/as/)
