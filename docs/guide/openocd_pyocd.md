# OpenOCD y PyOCD: Herramientas de Depuración y Programación

A lo largo de este curso, la carga de cada programa en la placa se realiza mediante el script `pico-flash`, que internamente recurre a uno de dos programas: **OpenOCD** o **PyOCD**. Esta guía explica qué es cada uno, cómo funcionan, y qué terminología conviene conocer para entender —más allá del script que la abstrae— qué ocurre realmente al programar el RP2040 por SWD.

## ¿Qué es un depurador en circuito (*on-chip debugger*)?

Un depurador en circuito es un sistema, compuesto de hardware y software, que permite a una computadora host controlar la ejecución de un microcontrolador objetivo sin necesidad de que este último ejecute ningún código adicional para ello. A través de un puerto de depuración dedicado (SWD o JTAG) y un adaptador físico —en este curso, el depurador CH552—, el host puede detener el núcleo, inspeccionar y modificar memoria y registros, colocar puntos de interrupción, ejecutar instrucción por instrucción, y —la operación más empleada en este curso— borrar y programar la memoria flash con un binario recién compilado. OpenOCD y PyOCD son, cada uno a su manera, implementaciones de software de este concepto general.

## OpenOCD (*Open On-Chip Debugger*)

OpenOCD es un proyecto de código abierto de propósito general para depuración y programación en circuito, con soporte para una variedad amplia de arquitecturas de procesador: ARM (Cortex-A, Cortex-R y Cortex-M), MIPS, RISC-V, Xtensa (empleado en los ESP32), entre otras. Su alcance no se limita a microcontroladores ARM, a diferencia de PyOCD.

OpenOCD funciona como un proceso servidor: por un lado, se comunica con el adaptador de depuración físico y traduce sus comandos al protocolo eléctrico correspondiente (SWD o JTAG); por otro, expone dos interfaces hacia el usuario o hacia otras herramientas:

- Un **servidor GDB**, que permite conectarse con un depurador interactivo estándar (`arm-none-eabi-gdb`) para inspeccionar el programa en ejecución línea por línea.
- Una **interfaz de comandos** basada en Tcl, accesible típicamente por telnet, que permite ejecutar operaciones puntuales —como programar la flash y reiniciar el objetivo— sin necesidad de abrir una sesión de depuración completa. Esta es la interfaz que emplea `pico-flash` para completar una carga de firmware de principio a fin.

Para operar, OpenOCD requiere dos archivos de configuración: uno que describe el tipo de adaptador de depuración conectado (por ejemplo, un adaptador CMSIS-DAP), y otro que describe el microcontrolador objetivo (registros de depuración, geometría de la memoria flash). El propio proyecto OpenOCD incluye, entre sus objetivos compatibles, soporte específico para el RP2040.

## PyOCD

PyOCD es un paquete de código abierto escrito en Python, enfocado exclusivamente en la programación y depuración de microcontroladores **ARM Cortex-M** mediante adaptadores compatibles con CMSIS-DAP —precisamente el tipo de adaptador que implementa el CH552 de este curso—. A diferencia de OpenOCD, PyOCD no pretende cubrir otras arquitecturas de procesador: su enfoque reducido le permite ofrecer una experiencia más simple y, en la práctica, tiempos de programación de flash frecuentemente más rápidos sobre objetivos Cortex-M.

La herramienta de línea de comandos `pyocd` ofrece varios subcomandos, entre los más relevantes:

- `gdbserver`: expone un servidor GDB, de manera equivalente a como lo hace OpenOCD.
- `load`: programa un archivo (`.elf`, `.bin`, `.hex`) directamente en la memoria flash o RAM del objetivo.
- `erase`: borra la totalidad o una parte de la memoria flash.
- `pack`: administra los *CMSIS-Packs*, paquetes de definición de dispositivo que extienden el soporte de PyOCD a prácticamente cualquier microcontrolador Cortex-M del mercado, más allá de los aproximadamente 70 dispositivos con soporte incorporado de fábrica.
- `commander`: una consola interactiva para inspeccionar y controlar el microcontrolador de manera manual.

Conforme a la documentación del propio proyecto, PyOCD se describe a sí mismo como una herramienta de calidad "beta" en lo que respecta a su API de Python (no así su herramienta de línea de comandos, de uso más estable); esto no ha impedido su adopción amplia como alternativa —o incluso reemplazo directo— de OpenOCD en flujos de trabajo basados en GDB.

## Diferencias clave entre ambas herramientas

| Aspecto | OpenOCD | PyOCD |
|---|---|---|
| Arquitecturas soportadas | ARM (A/R/M), MIPS, RISC-V, Xtensa, entre otras | Exclusivamente ARM Cortex-M |
| Lenguaje de implementación | C | Python |
| Configuración del objetivo | Archivos de script Tcl (interfaz + objetivo) | Soporte incorporado o CMSIS-Packs |
| Interfaces expuestas | Servidor GDB + consola de comandos Tcl (telnet) | Servidor GDB + CLI propia (`pyocd`) + API de Python |
| Madurez declarada | Proyecto establecido de larga trayectoria | Herramienta de línea de comandos estable; API de Python descrita como "beta" por sus propios mantenedores |
| Extensibilidad | Módulos en C, scripts Tcl | Biblioteca Python, fácil de incorporar en scripts o pruebas automatizadas propias |

En la práctica, ambas herramientas pueden operar el mismo adaptador CH552 sin distinción, precisamente porque este cumple con la especificación CMSIS-DAP (véase más abajo) — de ahí que, en las tablas de "Errores comunes" de varias prácticas de este curso, se mencione "OpenOCD o PyOCD, según el caso": cuál de las dos reporta el error depende únicamente de cuál esté configurada por detrás de `pico-flash` en el sistema del usuario.

## Terminología y componentes involucrados

- **SWD (*Serial Wire Debug*):** protocolo de depuración de dos líneas (reloj `SWCLK` y datos bidireccionales `SWDIO`), estándar en los núcleos ARM Cortex-M, empleado en este curso para conectar el CH552 con el RP2040.
- **JTAG:** protocolo de depuración anterior a SWD, con más líneas físicas (`TCK`, `TMS`, `TDI`, `TDO`), de uso extendido en la industria más allá del mundo ARM. OpenOCD lo soporta, pero no se emplea en este curso.
- **CMSIS-DAP:** especificación de ARM que define un protocolo estándar, sobre USB HID, para adaptadores de depuración. Un adaptador que cumple esta especificación —como el CH552— es reconocido por cualquier herramienta compatible (OpenOCD, PyOCD, o las herramientas propias de ARM) sin necesidad de controladores específicos del fabricante del adaptador.
- **Adaptador o sonda de depuración (*debug probe*):** el hardware físico que conecta, por un lado, al puerto USB de la computadora host y, por el otro, a las líneas SWD/JTAG del microcontrolador objetivo. El CH552 cumple este papel en este curso.
- **Algoritmo de flash (*flash algorithm*):** dado que un microcontrolador no puede escribir su propia memoria flash mientras ejecuta código directamente desde ella, tanto OpenOCD como PyOCD cargan temporalmente, en la RAM del objetivo, una rutina específica del fabricante del chip capaz de borrar y programar la flash; esta rutina se descarta una vez concluida la operación.
- **GDB (*GNU Debugger*) y servidor GDB:** GDB es el depurador interactivo estándar del ecosistema GCC; un "servidor GDB" —función que cumplen tanto OpenOCD como PyOCD— traduce el protocolo remoto de GDB hacia los comandos SWD/JTAG reales, permitiendo depurar un microcontrolador con las mismas órdenes (`break`, `step`, `continue`, `backtrace`) que se usarían para depurar un programa de escritorio.
- **ELF (*Executable and Linkable Format*):** formato del archivo compilado (`.elf`) que, a diferencia de `.bin` o `.hex`, conserva los símbolos de depuración (nombres de función, variables, líneas de código fuente); por ello, tanto los comandos manuales de OpenOCD como los del propio `pico-flash` programan y verifican a partir del `.elf`, no del `.bin`.
- **Archivo de configuración de objetivo (*target config*):** archivo que describe, ante la herramienta de depuración, las particularidades del microcontrolador conectado (geometría de flash, registros de depuración disponibles); en OpenOCD, este es el archivo `target/rp2040.cfg` referenciado en la guía de compilación.

## Recursos

- [OpenOCD — sitio oficial](https://openocd.org/)
- [PyOCD — repositorio oficial](https://github.com/pyocd/pyOCD)
- [Documentación de PyOCD](https://pyocd.io/)
- [CMSIS-DAP — especificación de ARM](https://arm-software.github.io/CMSIS_5/DAP/html/index.html)
