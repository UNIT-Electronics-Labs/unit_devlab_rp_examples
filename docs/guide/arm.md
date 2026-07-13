# Arquitectura ARM y su Comparación con la Familia PIC

Esta guía introduce la arquitectura ARM —la familia de procesadores sobre la que se construye el RP2040— y la sitúa en contexto frente a la familia PIC de Microchip, punto de partida habitual de quienes se incorporan a este curso. El objetivo no es argumentar que una arquitectura sea categóricamente superior a la otra, sino presentar las diferencias reales entre ambas para que cada docente pueda evaluar, con datos concretos, qué implicaciones tiene la transición hacia ARM en su propio contexto de enseñanza o desarrollo.

## ¿Qué es ARM?

ARM (originalmente *Acorn RISC Machine*, hoy *Advanced RISC Machine*) no es, en sí misma, una marca de microcontroladores como PIC, sino una **arquitectura de conjunto de instrucciones** (ISA) y un conjunto de diseños de núcleo de procesador, licenciados por la compañía ARM Holdings a decenas de fabricantes de semiconductores distintos. ARM Holdings no fabrica chips: diseña núcleos de CPU y los licencia a empresas como STMicroelectronics, NXP, Nordic Semiconductor, Texas Instruments, Renesas, Raspberry Pi (a través de Broadcom, y más recientemente con núcleos propios) y, de manera notable, la propia Microchip. Cada fabricante integra el núcleo licenciado junto con su propio conjunto de periféricos, memoria y características analógicas, para producir el microcontrolador final.

Esto contrasta directamente con la familia PIC: la arquitectura de los PIC10 a PIC18 es propiedad exclusiva de Microchip, implementada únicamente en silicio de Microchip, con un conjunto de instrucciones propietario que además varía entre familias (palabras de instrucción de 12 bits en PIC10/12, 14 bits en PIC16, 16 bits en PIC18). Un programa en ensamblador para PIC16 no es compatible con PIC18, y ninguno de los dos es compatible con el ensamblador de otro fabricante. Un programa en ensamblador ARM Cortex-M0+, en cambio, es compatible —a nivel de instrucciones base— con cualquier otro microcontrolador Cortex-M0+ del mercado, sin importar qué fabricante lo produjo.

## ¿Cómo funciona la arquitectura ARM?

Los núcleos ARM siguen la filosofía de diseño RISC (*Reduced Instruction Set Computer*): un conjunto de instrucciones deliberadamente reducido y de formato regular, en el que únicamente las instrucciones de carga y almacenamiento (`LDR`/`STR`) acceden a memoria, mientras que las operaciones aritméticas y lógicas trabajan exclusivamente sobre un banco uniforme de registros. Esta separación —opuesta a la filosofía CISC de arquitecturas como x86, donde una misma instrucción puede leer memoria, operar y escribir el resultado— simplifica el diseño del segmentado de instrucciones (*pipeline*) y facilita alcanzar mayor frecuencia de reloj con menor consumo.

Los núcleos orientados a microcontroladores (perfil *Cortex-M*, detallado más abajo) emplean el conjunto de instrucciones Thumb / Thumb-2: una codificación mixta de instrucciones de 16 y 32 bits que reduce el tamaño del código compilado frente al conjunto ARM de 32 bits empleado por los procesadores de aplicaciones, sin sacrificar demasiado desempeño. Todos los núcleos Cortex-M, independientemente del fabricante que los integre, comparten además un controlador de interrupciones estandarizado (NVIC, *Nested Vectored Interrupt Controller*), un temporizador de sistema (SysTick) y una interfaz de depuración estándar (SWD y/o JTAG) — precisamente la interfaz que emplea el depurador CH552 de este curso.

ARM organiza sus núcleos en tres perfiles de arquitectura, cada uno definido por un objetivo de uso distinto:

| Perfil | Núcleos representativos | Uso típico |
|---|---|---|
| **Cortex-A** (aplicaciones) | Cortex-A53, A76, X4 | Teléfonos inteligentes, tablets, sistemas que ejecutan Linux/Android |
| **Cortex-R** (tiempo real) | Cortex-R4, R5, R52 | Frenos automotrices, controladores de almacenamiento, banda base 5G |
| **Cortex-M** (microcontroladores) | Cortex-M0+, M4, M33... | Sensores, control de motores, dispositivos IoT — el perfil del RP2040 |

## Familias del perfil Cortex-M

El perfil Cortex-M agrupa varios núcleos, cada uno con un balance distinto entre costo, consumo y desempeño:

| Núcleo | Arquitectura | Unidad de punto flotante (FPU) | Perfil de uso |
|---|---|---|---|
| Cortex-M0 | Armv6-M | No | Menor costo/área de silicio de la familia |
| **Cortex-M0+** | Armv6-M | No | Como M0, pero con menor consumo y E/S de un solo ciclo (el núcleo del RP2040) |
| Cortex-M3 | Armv7-M | No | División por hardware, mayor desempeño de propósito general |
| Cortex-M4 | Armv7E-M | Opcional (precisión simple) | Instrucciones DSP, control de motores, procesamiento de señales |
| Cortex-M7 | Armv7E-M | Opcional (precisión simple o doble) | El más rápido de la línea "clásica"; caché e instrucción superescalar |
| Cortex-M23 | Armv8-M (base) | No | Sucesor del M0+, con TrustZone opcional |
| Cortex-M33 | Armv8-M (principal) | Opcional | Sucesor del M3/M4, con TrustZone opcional |
| Cortex-M55 / M85 | Armv8.1-M | Sí | Extensión vectorial Helium, orientados a inferencia de IA en el borde |

El RP2040 integra **dos** núcleos Cortex-M0+ —de ahí la práctica de Multicore— funcionando hasta 133 MHz. La elección de este núcleo específico no es casual: al ser el miembro más pequeño y eficiente de la familia Cortex-M, mantiene el costo y el consumo comparables a los de un microcontrolador de 8 bits, mientras conserva las ventajas de una arquitectura de 32 bits estándar y de un ecosistema de herramientas común a todo el mundo ARM.

## Comparación general: PIC clásico frente a RP2040 (Cortex-M0+)

| Aspecto | Familia PIC (8/16 bits clásica) | RP2040 (ARM Cortex-M0+) |
|---|---|---|
| Arquitectura | Propietaria de Microchip; Harvard; distinta por familia (PIC16/18/24) | Estándar abierta (Armv6-M), licenciada a múltiples fabricantes |
| Ancho de palabra nativo | 8 bits (PIC10/12/16/18) o 16 bits (PIC24/dsPIC) | 32 bits |
| Núcleos | Uno | Dos, simétricos |
| Frecuencia típica | Del orden de unos pocos MHz hasta ~64 MHz | Hasta 133 MHz |
| Toolchain | XC8/XC16/XC32, de Microchip | GCC estándar (`arm-none-eabi`), libre y multiplataforma |
| Depuración | ICSP propietario (PICkit) | SWD estándar, compatible con múltiples depuradores |
| Fabricantes que lo producen | Únicamente Microchip | Decenas (STMicroelectronics, NXP, Nordic, Raspberry Pi, entre otros) |
| Periférico distintivo | Módulos analógicos integrados (op-amps, comparadores) en varias familias | PIO: máquinas de estado programables para periféricos a medida |
| Punto flotante | Por software en la mayoría de las familias 8/16 bits | Por software en Cortex-M0+ (sin FPU); disponible en hardware desde Cortex-M4 |

## ¿Cuándo conviene cada arquitectura?

Presentar esta comparación de manera honesta exige reconocer que ninguna de las dos arquitecturas es categóricamente mejor: cada una resuelve bien problemas distintos.

**Razones objetivas a favor de considerar RP2040/Cortex-M0+:**

- El ancho de palabra de 32 bits maneja de manera nativa operaciones aritméticas (marcas de tiempo, escalado de lecturas ADC) que en un PIC de 8 bits requieren fragmentarse en varias operaciones de registro.
- El doble núcleo permite paralelismo real de hardware (práctica de Multicore) sin depender de un sistema operativo de tiempo real.
- El PIO permite implementar protocolos o periféricos a la medida sin ocupar tiempo de CPU —una capacidad sin equivalente directo en la familia PIC clásica.
- El toolchain es gratuito, abierto, y las habilidades adquiridas (C con GCC, SWD, NVIC) son directamente transferibles a prácticamente cualquier otro microcontrolador Cortex-M del mercado, no solo al RP2040.
- La comunidad y la cantidad de ejemplos disponibles —incluyendo Arduino, CircuitPython, Zephyr y FreeRTOS sobre el mismo silicio— es considerablemente mayor y no depende de un único fabricante.

**Razones por las que un PIC clásico puede seguir siendo la opción correcta:**

- Para diseños de muy bajo número de pines (PIC10/12 en encapsulados de 6 u 8 pines), el RP2040 resulta sobredimensionado.
- Algunas familias PIC de bajo consumo (XLP) alcanzan corrientes de reposo del orden de nanoamperes, relevante en diseños alimentados por batería durante años.
- Varias familias PIC integran periféricos analógicos (amplificadores operacionales, comparadores, DAC) directamente en el silicio, reduciendo el conteo de componentes externos en diseños con carga analógica intensiva.
- La inversión existente en herramientas, código y experiencia del equipo con el ecosistema MPLAB/PIC tiene un valor real que no debe descartarse solo por antigüedad de la arquitectura.

Vale la pena señalar, como dato adicional y no como argumento de un lado u otro, que la propia Microchip ha diversificado su oferta de 32 bits más allá de su arquitectura propietaria: la familia **PIC32C** ya se basa en núcleos ARM Cortex-M0+, la línea **PIC32M** emplea núcleos MIPS32, y en 2025 Microchip presentó **PIC32A**, con un conjunto de instrucciones propio pero explícitamente pensado para convivir junto a las variantes ARM y MIPS dentro del mismo catálogo. La tendencia de la industria —incluyendo al propio fabricante de PIC— ha sido diversificar hacia arquitecturas estándar para sus productos de mayor gama, reservando las familias propietarias de 8 y 16 bits para los casos de uso más simples y de menor costo.

## Recursos

- [Arm Cortex-M — documentación oficial](https://developer.arm.com/Processors/Cortex-M)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [PIC Microcontrollers — Microchip](https://www.microchip.com/en-us/products/microcontrollers/8-bit-mcus/pic-mcus)
- [Comparación de familias PIC32 — Microchip](https://www.microchip.com/en-us/products/microcontrollers/32-bit-mcus)
