# CMakeLists.txt

Todo proyecto basado en el Raspberry Pi Pico SDK depende de un único `CMakeLists.txt` que decide qué se compila, para qué placa, y con qué librerías del SDK se enlaza. A diferencia de un `CMakeLists.txt` genérico de C/C++, este trae varias funciones propias del SDK (`pico_sdk_init`, `pico_add_extra_outputs`, etc.) que no existen en CMake puro.

```cmake
cmake_minimum_required(VERSION 3.20)

include("${CMAKE_CURRENT_LIST_DIR}/../pico_sdk_import.cmake")

set(PROJECT_NAME    "mi_proyecto")
set(PROJECT_SOURCES "mi_proyecto.c") 
set(PICO_BOARD      "pico")

set(PROJECT_LIBS
    pico_stdlib
    hardware_adc
)

project(${PROJECT_NAME}  C CXX ASM)

pico_sdk_init()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})

pico_add_extra_outputs(${PROJECT_NAME})
```

---

## `cmake_minimum_required(VERSION 3.20)`

Fija la versión mínima de CMake compatible con las funciones del SDK. Las versiones más recientes del pico-sdk (2.x) validan además un rango superior — por ejemplo `VERSION 3.13...3.27` — para advertir si CMake ya cambió de comportamiento en versiones muy nuevas. Sin esta línea, un CMake demasiado viejo falla más adelante con errores confusos en vez de indicar directamente la incompatibilidad.

## `include("${CMAKE_CURRENT_LIST_DIR}/../pico_sdk_import.cmake")`

Localiza el SDK en el sistema, antes de que exista cualquier noción de "proyecto" para CMake. Internamente busca, en este orden de prioridad:

- La variable de entorno `PICO_SDK_PATH`
- Una variable de CMake ya cacheada con ese mismo nombre
- Una carpeta `pico-sdk/` junto al proyecto (patrón de submódulo)
- Si `PICO_SDK_FETCH_FROM_GIT` está activado, lo clona automáticamente desde GitHub

Debe ir siempre antes de `project()`. Es la única regla de orden verdaderamente estricta en todo el archivo — casi todo lo demás se acomoda solo.

> **Importante — cambio de comportamiento desde el SDK 2.0.0:** si tu proyecto necesita fijar `PICO_BOARD` a algo distinto de `pico` (por ejemplo `pico2` o `pico_w`), esa línea debe ir **antes** de este `include`, no después. En versiones anteriores del SDK bastaba con ponerlo antes de `pico_sdk_init()`, pero desde la 2.0.0 el valor se ignora silenciosamente si se declara después del `include` — el build compila igual, sin error, pero como si fuera un `pico` normal.

## `set(PROJECT_NAME "mi_proyecto")`

Variable interna de conveniencia, no reservada por CMake ni por el SDK. Se reutiliza en el resto del archivo (`project()`, `add_executable()`, etc.) para que cambiar el nombre del proyecto se reduzca a editar una sola línea.

## `set(PROJECT_SOURCES "mi_proyecto.c")` o `set(PROJECT_SOURCES "mi_proyecto.c mi_proyecto2.c")`

Lista de archivos fuente que se compilan. Para proyectos de un solo archivo basta con la cadena directa; si el proyecto crece a varios `.c`/`.cpp`, se convierte en lista:

```cmake
set(PROJECT_SOURCES
    main.c
    sensores.c
    utilidades.c
)
```

## `set(PICO_BOARD "pico")`

Le indica al SDK para qué placa física se compila. Este valor determina, entre otras cosas: los pines por defecto de UART/I2C, si se incluye o no el soporte de radio (Wi-Fi/Bluetooth), y qué plataforma de chip (`PICO_PLATFORM`) se usa por debajo.

| Valor | Chip | Wireless | Uso típico |
|---|---|---|---|
| `pico` | RP2040 | No | Placa original, la más común |
| `pico_w` | RP2040 | Sí (Wi-Fi/BT vía CYW43) | Proyectos con conectividad |
| `pico2` | RP2350 | No | Nueva generación, más rápida y con más RAM |
| `pico2_w` | RP2350 | Sí | Nueva generación + conectividad |

El SDK también trae definiciones para decenas de placas de terceros (Adafruit, SparkFun, Pimoroni, Olimex, entre otras) — la lista completa vive en el propio código fuente del SDK, en `src/boards/include/boards/`, un archivo de cabecera por cada placa soportada.

## `set(PROJECT_LIBS pico_stdlib ...)`

Librerías del SDK que se enlazan al ejecutable. `pico_stdlib` es la base casi universal (incluye `stdio`, GPIO básico, `sleep_ms`, etc.); el resto se agrega según el periférico que use el código.

| Periférico | Librería a enlazar |
|---|---|
| GPIO digital | `hardware_gpio` |
| PWM | `hardware_pwm` |
| I2C | `hardware_i2c` |
| SPI | `hardware_spi` |
| UART | `hardware_uart` |
| ADC | `hardware_adc` |
| Timers / alarmas | `hardware_timer` |
| Interrupciones | `hardware_irq` |

Usar una función de un periférico en el código (`adc_init()`, `i2c_write_blocking()`, etc.) sin haber agregado su librería correspondiente aquí no rompe la compilación — el código compila normal — pero falla en la etapa de *linking* con errores de símbolo no encontrado.

## `project(${PROJECT_NAME}  C CXX ASM)`

Declara formalmente el proyecto ante CMake. A partir de este punto CMake ya identifica compiladores disponibles. No hace falta declarar lenguajes explícitos (`project(nombre C CXX ASM)`) — `pico_sdk_init()`, más abajo, se encarga de habilitarlos internamente.

## `pico_sdk_init()`

El corazón de la integración con el SDK. Agrega como subdirectorio todo el árbol de `pico-sdk`, expone cada librería (`pico_stdlib`, `hardware_*`, etc.) como *target* de CMake disponible para enlazar, y configura la cadena de compilación cruzada para ARM (RP2040) o ARM/RISC-V (RP2350). Siempre después de `project()` — llamarlo antes genera una advertencia y comportamiento no garantizado.

## `add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})`

Comando estándar de CMake (no específico del SDK): declara el binario final y de qué fuentes se compila.

## `target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})`

Conecta el ejecutable con las librerías declaradas en `PROJECT_LIBS`. Aquí es donde el *linker* resuelve las llamadas del código con su implementación real dentro del SDK.

## `pico_add_extra_outputs(${PROJECT_NAME})`

Función propia del pico-sdk. A partir del `.elf` compilado, genera automáticamente los formatos necesarios para trabajar con el hardware:

| Archivo | Para qué sirve |
|---|---|
| `.uf2` | Flasheo por arrastrar-y-soltar en modo BOOTSEL |
| `.bin` | Imagen binaria cruda |
| `.hex` | Formato Intel HEX, usado por algunos programadores externos |
| `.map` | Mapa de memoria, útil para depurar uso de RAM/Flash |

Sin esta línea solo se genera el `.elf` — válido para depurar por SWD/OpenOCD, pero no usable para flasheo directo.

---

## Configuración opcional de `stdio`

No forma parte de la plantilla base, pero es común agregarla cuando se necesita consola por USB en vez de por los pines físicos de UART:

```cmake
pico_enable_stdio_usb(${PROJECT_NAME} 1)   # activa consola por USB (CDC serial)
pico_enable_stdio_uart(${PROJECT_NAME} 0)  # desactiva salida por pines UART físicos
```

Por defecto, sin estas dos líneas, `pico_stdlib` envía `printf()` por UART (pines TX/RX físicos). Se activa USB cuando se quiere ver la consola por el mismo cable que ya se usa para flashear, sin conectar un adaptador UART externo.
