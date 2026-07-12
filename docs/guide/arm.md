# Arquitectura de Microcontroladores: El Ecosistema ARM Cortex-M

El estudio de los sistemas embebidos requiere comprender no solo cómo escribir código, sino cómo el procesador interactúa con la memoria y los periféricos a nivel de silicio. En este curso utilizamos el **RP2040**, un microcontrolador moderno que nos permite explorar conceptos de alto rendimiento manteniendo un entorno accesible para el laboratorio.

Antes de manipular los registros físicos, es indispensable establecer el marco de referencia arquitectónico sobre el cual trabajaremos: la familia **ARM Cortex-M**.

## ¿Qué es ARM y la familia Cortex-M?

A diferencia de otros fabricantes de silicio, **ARM** no fabrica microcontroladores. ARM diseña arquitecturas de procesadores (propiedad intelectual o IP) y las licencia a fabricantes como NXP, STMicroelectronics, Texas Instruments y, en este caso, Raspberry Pi. Esto significa que **aprender a programar un núcleo ARM garantiza que el conocimiento es transferible** a miles de microcontroladores de docenas de fabricantes distintos en la industria.

Dentro de su catálogo, ARM divide sus procesadores en perfiles. El **Perfil M (Cortex-M)** está diseñado estrictamente para **Microcontroladores**. Sus características principales son:
* **Baja latencia:** Respuestas deterministas e inmediatas a interrupciones.
* **Eficiencia energética:** Diseñados para operar en entornos con restricciones de energía.
* **Conjunto de instrucciones Thumb:** Un set de instrucciones RISC optimizado de 16 y 32 bits que maximiza la densidad del código (hace más con menos memoria).

## El Núcleo Cortex-M0+ en el RP2040

El RP2040 cuenta con un procesador **Dual-Core ARM Cortex-M0+** operando hasta a 133 MHz. El Cortex-M0+ es el núcleo más pequeño y eficiente de toda la familia ARM, diseñado específicamente para ser el puente de transición ideal desde las arquitecturas clásicas de 8 y 16 bits hacia el mundo de los 32 bits.

Sus atributos más relevantes para nuestras prácticas incluyen:
1. **Arquitectura Von Neumann (con matriz de buses):** A diferencia de la arquitectura Harvard estricta, aquí las instrucciones y los datos comparten el mismo mapa de memoria. El RP2040 soluciona el cuello de botella tradicional de Von Neumann mediante un *crossbar* (matriz de conmutación) que permite accesos simultáneos a distintas áreas de la memoria RAM.
2. **Mapa de memoria plano de 32 bits:** Todo (RAM, Flash, periféricos y registros de control) se encuentra mapeado en un único espacio de direcciones lineal de 4 Gigabytes. 
3. **Pipeline de 2 etapas:** Un cauce de instrucciones ultracorto que garantiza que los saltos condicionales (`branch`) tengan una penalización mínima en ciclos de reloj.

## Transición: De 8 bits (PIC) a 32 bits (ARM)

Durante décadas, las arquitecturas de 8 bits (como la familia PIC16/PIC18 de Microchip) han sido el pilar fundamental tanto en la industria como en la academia. Su robustez eléctrica y su arquitectura predecible han formado a generaciones de ingenieros. 

Sin embargo, a medida que los requerimientos de procesamiento de señales, criptografía y comunicaciones de red aumentan, la industria ha migrado de forma masiva hacia soluciones de 32 bits. La adopción del Cortex-M0+ en el aula no invalida los conceptos aprendidos con PIC; por el contrario, los expande. 

A continuación, se contrastan algunas diferencias clave desde una perspectiva pedagógica:

| Característica | Arquitecturas Clásicas de 8 bits (ej. PIC tradicional) | Arquitectura ARM Cortex-M0+ (32 bits) | Ventaja Didáctica |
| :--- | :--- | :--- | :--- |
| **Ancho de Palabra** | 8 bits. Operaciones con números mayores a 255 (ej. un ADC de 12 bits) requieren múltiples ciclos de reloj y manejo de acarreos. | 32 bits. Operaciones con números hasta 4,294,967,295 se resuelven de forma nativa en un solo ciclo de reloj. | El alumno se enfoca en la matemática del proyecto (procesamiento de señales, filtros) sin lidiar con los límites aritméticos del hardware. |
| **Gestión de Memoria** | Memoria segmentada en bancos. Requiere instrucciones constantes de cambio de banco (ej. `BANKSEL`) para acceder a distintos registros. | Mapa de memoria plano. Cualquier registro o variable está disponible directamente mediante un puntero de 32 bits. | Se elimina el riesgo de corrupción de datos por un cambio de banco olvidado, facilitando el uso intensivo del lenguaje C. |
| **Interrupciones** | Generalmente un solo vector de interrupción (o dos en familias avanzadas). El software debe sondear banderas para saber qué periférico interrumpió. | Hardware dedicado (**NVIC** - *Nested Vectored Interrupt Controller*). Cada periférico tiene su propio vector y prioridad gestionada por hardware. | Permite enseñar conceptos de sistemas operativos de tiempo real (RTOS), concurrencia y anidación de tareas críticas de forma nativa. |
| **Multiplicación** | Por software o mediante módulos limitados de hardware de un ciclo. | Multiplicador por hardware de 32 bits en un solo ciclo (característica estándar en el M0+ del RP2040). | Viabilidad para implementar algoritmos rápidos de DSP (Digital Signal Processing) y lazos de control PID eficientes. |

::: info El valor de la estandarización
Migrar a ARM elimina la dependencia de compiladores propietarios. Al usar el Cortex-M0+, las prácticas se compilan utilizando **GCC (GNU Compiler Collection)**, el mismo estándar industrial utilizado para compilar software en sistemas Linux y servidores, ofreciendo una optimización de código inigualable.
:::

## El Paradigma Multicore y los bloques PIO

Más allá de las ventajas estándar de ARM, el RP2040 introduce características avanzadas que justifican su selección como plataforma de estudio:

1. **Dual-Core:** Dispone de dos núcleos Cortex-M0+ idénticos. Esto permite enseñar paralelismo asíncrono real; por ejemplo, el Núcleo 0 puede encargarse de la lectura crítica de un sensor analógico, mientras el Núcleo 1 gestiona la actualización de una pantalla OLED sin interrumpirse mutuamente.
2. **Entrada/Salida Programable (PIO):** Una de las mayores limitaciones al enseñar protocolos es depender de periféricos fijos o recurrir al *bit-banging* (control manual de pines que consume todos los recursos del CPU). El RP2040 incluye coprocesadores de estado (PIO) que los estudiantes pueden programar en un ensamblador simplificado para crear hardware personalizado (como interfaces DVI, controladores de matrices LED o protocolos experimentales) liberando por completo a los núcleos ARM principales.

Comprender esta arquitectura es el primer paso. En las siguientes secciones, dejaremos la teoría de lado para interactuar directamente con los registros físicos de este silicio a través del **Pico C/C++ SDK**.