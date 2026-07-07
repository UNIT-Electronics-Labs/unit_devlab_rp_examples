# Configuración del SDK para RP2040

Esta guía te ayudará a configurar el entorno de desarrollo para programar el RP2040 en C y ensamblador.

## Requisitos

- **CMake** 3.13 o superior
- **Compilador ARM GCC** (arm-none-eabi-gcc)
- **Pico SDK** (pico-sdk)
- **Node.js** 18+ (opcional, para documentación VitePress)
- Placa con RP2040 (Raspberry Pi Pico, Pico W, etc.)
- Cable USB para programación

## Instalación en Linux

### Ubuntu/Debian

```bash
# Instalar dependencias
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi \
  build-essential libstdc++-arm-none-eabi-newlib git

# Clonar el SDK de Pico
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Configurar variable de entorno
echo 'export PICO_SDK_PATH=$HOME/pico-sdk' >> ~/.bashrc
source ~/.bashrc
```

### Fedora/RHEL

```bash
sudo dnf install cmake arm-none-eabi-gcc-cs arm-none-eabi-newlib git

cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

echo 'export PICO_SDK_PATH=$HOME/pico-sdk' >> ~/.bashrc
source ~/.bashrc
```

## Instalación en macOS

```bash
# Instalar Homebrew si no lo tienes
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Instalar herramientas
brew install cmake
brew tap ArmMbed/homebrew-formulae
brew install arm-none-eabi-gcc

# Clonar el SDK
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

echo 'export PICO_SDK_PATH=$HOME/pico-sdk' >> ~/.zshrc
source ~/.zshrc
```

## Instalación en Windows

Consulta la [Guía para Windows](./windows.md) para instrucciones detalladas.

## Herramientas Opcionales

### picotool

Herramienta para cargar programas sin modo BOOTSEL:

```bash
# Linux
git clone https://github.com/raspberrypi/picotool.git
cd picotool
mkdir build && cd build
cmake ..
make
sudo make install
```

### minicom o screen

Para comunicación serial:

```bash
# Linux
sudo apt install minicom
# o
sudo apt install screen

# Usar
minicom -D /dev/ttyACM0 -b 115200
# o
screen /dev/ttyACM0 115200
```

## Compilar un Ejemplo

```bash
cd ejemplo_proyecto
mkdir build
cd build
cmake ..
make
```

Esto genera un archivo `.uf2` listo para cargar.

## Cargar el Programa

### Método 1: Modo BOOTSEL (más simple)

1. Desconecta la placa
2. Mantén presionado el botón BOOTSEL
3. Conecta el USB (manteniendo BOOTSEL)
4. Suelta BOOTSEL
5. La placa aparece como unidad de almacenamiento
6. Copia el archivo `.uf2`

### Método 2: Con picotool

```bash
picotool load -f programa.uf2
picotool reboot
```

## Verificar Instalación

```bash
# Verificar compilador
arm-none-eabi-gcc --version

# Verificar CMake
cmake --version

# Verificar SDK
echo $PICO_SDK_PATH
```

## Crear Proyecto Nuevo

### 1. Crear estructura de directorios

```bash
mkdir mi_proyecto
cd mi_proyecto
```

### 2. Copiar pico_sdk_import.cmake

```bash
cp $PICO_SDK_PATH/external/pico_sdk_import.cmake .
```

### 3. Crear CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(mi_proyecto C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(mi_proyecto
    main.c
)

target_link_libraries(mi_proyecto
    pico_stdlib
)

pico_add_extra_outputs(mi_proyecto)
pico_enable_stdio_usb(mi_proyecto 1)
pico_enable_stdio_uart(mi_proyecto 0)
```

### 4. Crear main.c

```c
#include "pico/stdlib.h"

int main() {
    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}
```

### 5. Compilar

```bash
mkdir build && cd build
cmake ..
make
```

## Solución de Problemas

### SDK no encontrado

```bash
# Verificar que PICO_SDK_PATH esté configurado
echo $PICO_SDK_PATH

# Si está vacío, configurar
export PICO_SDK_PATH=/ruta/a/pico-sdk
```

### Error de compilador

```bash
# Verificar que arm-none-eabi-gcc esté en PATH
which arm-none-eabi-gcc

# Si no se encuentra, instalar
sudo apt install gcc-arm-none-eabi
```

### Puerto serial no aparece

```bash
# Linux: agregar usuario al grupo dialout
sudo usermod -a -G dialout $USER
# Cerrar sesión y volver a entrar

# macOS: no requiere permisos adicionales

# Windows: verificar drivers USB en Device Manager
```

```bash
cd digital-labs/01_sumador_n_bits
devlab build
```

::: tip Windows
En Windows, usa `python -m devlab build` en lugar de `devlab build`.
:::

`devlab.toml` usa Verilog por defecto:

```toml
[build]
top = "top"
sources = ["src/digital_labs.v", "src/top.v"]
constraints = "pins.cst"
build_dir = "build"
```

## Compilar VHDL

```bash
cd digital-labs/01_sumador_n_bits
devlab build -c devlab-vhdl.toml
```

`devlab-vhdl.toml` usa la versión VHDL autocontenida:

```toml
[build]
top = "top"
sources = ["src/top.vhd"]
constraints = "pins.cst"
build_dir = "build"
```

## Cargar en SRAM

```bash
devlab flash
```

La configuración actual carga en SRAM y usa una tarjeta de referencia:

```toml
[flash]
board = "tangnano9k"
mode = "sram"
verify = false
```

`board` identifica el target que usará la herramienta de programación. No es parte del diseño HDL: si usas otra tarjeta con el mismo chip o con otro dispositivo Gowin compatible, cambia esta sección y el archivo `pins.cst` correspondiente.

## Estructura de un ejemplo

```text
ejemplo/
|-- devlab.toml
|-- devlab-vhdl.toml
|-- pins.cst
|-- src/
|   |-- top.v
|   |-- top.vhd
|   `-- digital_labs.v
`-- README.md
```

Los ejemplos básicos no siempre necesitan `digital_labs.v`; los laboratorios digitales sí lo usan para separar módulos reutilizables del `top`.

## Solución de Problemas

### Windows: Error en nextpnr-himbaechel

Si obtienes un error `CalledProcessError` con `nextpnr-himbaechel.exe` retornando código de salida 125:

```
subprocess.CalledProcessError: Command '['...\nextpnr-himbaechel.exe', ...]' returned non-zero exit status 125.
```

**Causa**: Windows Defender está bloqueando o ralentizando las herramientas de síntesis.

**Solución**: Configura las exclusiones de Windows Defender según se describe en la sección [Configuración Adicional para Windows](#configuracion-adicional-para-windows).

### Verificar que DevLab funciona correctamente

```bash
devlab doctor
```

En Windows:

```powershell
python -m devlab doctor
```

Este comando verifica que todos los toolchains estén instalados correctamente.

## Siguiente Paso

Después de entender el build, revisa [Archivos CST](./cst.md) para aprender cómo conectar entradas y salidas del HDL con pines físicos de la placa.
