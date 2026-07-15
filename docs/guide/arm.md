# Arquitectura ARM y su Comparación con la Familia PIC

Esta guía introduce la arquitectura ARM —la familia de procesadores sobre la que se construye el RP2040— y la sitúa en contexto frente a la familia PIC de Microchip, punto de partida habitual de quienes se incorporan a este curso. El objetivo no es argumentar que una arquitectura sea categóricamente superior a la otra, sino presentar las diferencias reales entre ambas para que cada docente pueda evaluar, con datos concretos, qué implicaciones tiene la transición hacia ARM en su propio contexto de enseñanza o desarrollo.

## ¿Qué es ARM?

ARM (originalmente *Acorn RISC Machine*, hoy *Advanced RISC Machine*) no es, en sí misma, una marca de microcontroladores como PIC, sino una **arquitectura de conjunto de instrucciones** (ISA) y un conjunto de diseños de núcleo de procesador, licenciados por la compañía ARM Holdings a decenas de fabricantes de semiconductores distintos. ARM Holdings no fabrica chips: diseña núcleos de CPU y los licencia a empresas como STMicroelectronics, NXP, Nordic Semiconductor, Texas Instruments, Renesas, Raspberry Pi (a través de Broadcom, y más recientemente con núcleos propios) y, de manera notable, la propia Microchip. Cada fabricante integra el núcleo licenciado junto con su propio conjunto de periféricos, memoria y características analógicas, para producir el microcontrolador final.

Esto contrasta directamente con la familia PIC: la arquitectura de los PIC10 a PIC18 es propiedad exclusiva de Microchip, implementada únicamente en silicio de Microchip, con un conjunto de instrucciones propietario que además varía entre familias (palabras de instrucción de 12 bits en PIC10/12, 14 bits en PIC16, 16 bits en PIC18). Un programa en ensamblador para PIC16 no es compatible con PIC18, y ninguno de los dos es compatible con el ensamblador de otro fabricante. Un programa en ensamblador ARM Cortex-M0+, en cambio, es compatible —a nivel de instrucciones base— con cualquier otro microcontrolador Cortex-M0+ del mercado, sin importar qué fabricante lo produjo.

## RISC vs. CISC: qué significa la "R" de ARM

Antes de comparar ARM con PIC conviene aclarar un punto que suele generar confusión: **ambas arquitecturas son RISC**. La distinción RISC/CISC no es, por tanto, la que separa a PIC de ARM — es una filosofía de diseño que ambas comparten frente a arquitecturas CISC como x86.

Un **RISC** (*Reduced Instruction Set Computer*) se caracteriza por:

- Un conjunto de instrucciones pequeño, de formato fijo o casi fijo, y de ejecución predecible.
- El principio *load-store*: solo las instrucciones de carga y almacenamiento acceden a memoria; el resto opera únicamente sobre registros internos del procesador.
- Instrucciones que, en su mayoría, se ejecutan en un solo ciclo de reloj.
- Un diseño que traslada complejidad del hardware al compilador o al programador, a cambio de un pipeline más simple y más rápido.

Un **CISC** (*Complex Instruction Set Computer*, como x86) permite en cambio instrucciones que combinan varias operaciones —leer de memoria, operar y escribir el resultado— en una sola instrucción, de longitud variable y ejecución en múltiples ciclos.

La familia PIC, desde sus orígenes en General Instrument, fue diseñada explícitamente bajo filosofía RISC: los PIC de gama media (PIC16) trabajan con un conjunto reducido de 35 instrucciones, y los PIC18 con 77 instrucciones —en ambos casos, palabras de instrucción de ancho fijo, ejecución en un ciclo para la mayoría de los casos (dos ciclos para saltos), y un diseño *load-store* similar en espíritu al de ARM. Los núcleos ARM Cortex-M0+ llevan esta misma filosofía a su expresión más depurada: **56 instrucciones** en el subconjunto Thumb/Thumb-2 que implementan, la mayoría de 16 bits.

Lo que sí distingue a ambas arquitecturas, y con más peso práctico para la migración PIC → ARM, no es RISC-vs-CISC sino los puntos que se detallan a continuación: ancho de palabra, modelo de memoria y organización del banco de registros.

## Modelo de memoria: Harvard puro frente a Von Neumann con bus único

Aquí aparece un dato que suele sorprender a quien migra de PIC: **toda la familia PIC utiliza arquitectura Harvard**, con buses físicamente separados para memoria de programa y memoria de datos, lo que permite leer una instrucción y acceder a un dato en el mismo ciclo. El RP2040, en cambio, implementa un núcleo Cortex-M0+ que utiliza **arquitectura Von Neumann**, con una única interfaz de bus tipo AMBA AHB-Lite que sirve tanto a instrucciones como a datos.

| Aspecto | Familia PIC | Cortex-M0+ (RP2040) |
|---|---|---|
| Modelo de memoria | Harvard (buses de programa y datos separados) | Von Neumann (bus único AHB-Lite) |
| Acceso simultáneo instrucción + dato | Sí, en el mismo ciclo | No; comparten la misma interfaz de bus |
| Ancho del bus de instrucción | Igual al ancho de la palabra de instrucción (12/14/16 bits), independiente del bus de datos de 8 bits | 32 bits, compartido con datos |

Esto no significa que el Cortex-M0+ sea más lento por diseño: el núcleo compensa la ausencia de buses separados con un **pipeline de solo 2 etapas** (Fetch y Execute) —el más corto de toda la familia Cortex-M, incluso más corto que el de un Cortex-M0 o un Cortex-M3/M4, que usan 3 etapas—, precisamente para minimizar el impacto de no tener acceso paralelo a memoria de programa y datos. Este pipeline reducido, además, disminuye el número de accesos a memoria flash por instrucción, lo cual explica en buena parte por qué el Cortex-M0+ logra una eficiencia energética competitiva (cifras publicadas por fabricantes de silicio como Silicon Labs reportan del orden de 11.2 µW/MHz) pese a no tener la ventaja estructural de un bus Harvard.

Vale la pena señalar que otros núcleos Cortex sí emplean variantes Harvard o Harvard modificado a nivel de sistema (separando instrucción y datos mediante caché, por ejemplo en Cortex-M7), por lo que "Von Neumann" no es un rasgo de toda la familia ARM, sino específicamente del Cortex-M0+ y del M0 que le precede.

## Modelo de registros: banco uniforme frente a acumulador con bancos de memoria

La segunda diferencia estructural, y la que más afecta directamente la forma de programar en ensamblador, es la organización del banco de registros.

**PIC (arquitectura clásica de acumulador):** el CPU opera alrededor de un único registro de trabajo, **W** (*Working register*), que funciona como acumulador: la gran mayoría de las instrucciones aritméticas y lógicas toman un operando de la memoria de datos (identificada como registro de archivo, *file register*, **f**) y lo combinan con W, depositando el resultado en W o de vuelta en f según el bit **d** de la instrucción. El acceso a la memoria de datos, además, está organizado en **bancos** (bank switching): el PIC16F877A, por ejemplo, requiere manipular el bit RP0 del registro STATUS para seleccionar el banco activo antes de acceder a ciertos registros, y el PIC18 extiende esto a un esquema de hasta 16 bancos de 256 bytes seleccionados mediante el registro BSR (*Bank Select Register*). Programar rutinas que crucen bancos exige instrucciones adicionales solo para cambiar de banco.

**ARM Cortex-M0+ (banco de registros plano):** el núcleo expone **16 registros de 32 bits (R0-R15)**, de los cuales R0-R12 son de propósito general y no requieren selección de banco — cualquier instrucción puede operar directamente entre dos de ellos sin pasos intermedios de conmutación. Los tres registros restantes tienen función arquitectónica fija: R13 es el puntero de pila (**SP**), R14 es el registro de enlace (**LR**, que guarda automáticamente la dirección de retorno en cada llamada a subrutina) y R15 es el contador de programa (**PC**). Dentro de R0-R12 existe una subdivisión relevante para el código Thumb de 16 bits: R0-R7 (*low registers*) son accesibles por prácticamente todas las instrucciones de 16 bits, mientras que R8-R12 (*high registers*) solo son accesibles desde instrucciones de 32 bits o un subconjunto reducido de instrucciones de 16 bits — una limitación de codificación, no de arquitectura de memoria.

| Aspecto | PIC (W + file registers) | Cortex-M0+ (R0-R15) |
|---|---|---|
| Registro(s) de trabajo | Uno (W), modelo acumulador | 13 de propósito general (R0-R12) |
| Selección de banco para acceder a memoria/registros | Sí (bit RP0 en PIC16; BSR de 4 bits en PIC18) | No aplica; banco plano |
| Puntero de pila dedicado en hardware | No en PIC16 clásico (pila de 8 niveles no direccionable por software); PIC18 sí expone pila accesible | Sí, dedicado (R13/SP), con soporte MSP/PSP |
| Registro de retorno de subrutina dedicado | No; usa una pila de hardware interna | Sí, dedicado (R14/LR) |
| Operandos por instrucción aritmética típica | Dos (W y un file register), resultado a W o f | Dos o tres registros de un banco uniforme |

Esta diferencia tiene consecuencias prácticas directas para quien migra: en PIC, buena parte del "arte" de escribir ensamblador eficiente consiste en administrar bancos y minimizar movimientos hacia/desde W; en Cortex-M0+, el compilador de C —y el programador de ensamblador— dispone de un espacio de trabajo mucho mayor y sin esa gestión de bancos, aunque a cambio debe atender la distinción low/high registers al escribir Thumb de 16 bits directamente.

## ¿Cómo funciona la arquitectura ARM?

Sobre la base RISC y el modelo de registros ya descritos, los núcleos ARM añaden dos rasgos que determinan cómo se ve el ensamblador en la práctica y cómo se organiza el catálogo completo de la familia.

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

El RP2040 integra **dos** núcleos Cortex-M0+ —de ahí la práctica de Multicore—. La elección de este núcleo específico no es casual: al ser el miembro más pequeño y eficiente de la familia Cortex-M, mantiene el costo y el consumo comparables a los de un microcontrolador de 8 bits, mientras conserva las ventajas de una arquitectura de 32 bits estándar y de un ecosistema de herramientas común a todo el mundo ARM.

## Comparación general: PIC clásico frente a RP2040 (Cortex-M0+)

| Aspecto | Familia PIC (8/16 bits clásica) | RP2040 (ARM Cortex-M0+) |
|---|---|---|
| Arquitectura | Propietaria de Microchip; Harvard; distinta por familia (PIC16/18/24) | Estándar abierta (Armv6-M), licenciada a múltiples fabricantes |
| Filosofía de conjunto de instrucciones | RISC (35 instrucciones en PIC16, 77 en PIC18) | RISC (56 instrucciones en el subconjunto Thumb/Thumb-2 de Armv6-M) |
| Modelo de memoria | Harvard (bus de programa y bus de datos separados) | Von Neumann (bus único AHB-Lite) |
| Modelo de registros | Acumulador único (W) + memoria de datos por bancos | Banco plano de 13 registros de propósito general (R0-R12) + SP/LR/PC dedicados |
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
