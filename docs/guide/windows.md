# Guía para Windows - RP2040

Esta guía proporciona pasos específicos para configurar el entorno de desarrollo RP2040 en Windows.

## Instalación Rápida

### 1. Instalar Herramientas Básicas

#### Opción A: Instalador Automático (Recomendado)

Raspberry Pi proporciona un instalador automático para Windows:

1. Descarga el [Pico Setup para Windows](https://github.com/raspberrypi/pico-setup-windows/releases/latest)
2. Ejecuta el instalador `pico-setup-windows-x64-standalone.exe`
3. Sigue el asistente (instalará CMake, ARM GCC, Build Tools, Git, Python, VS Code)
4. Reinicia el sistema cuando termine

#### Opción B: Instalación Manual

```powershell
# Instalar CMake
winget install Kitware.CMake

# Instalar Build Tools para Visual Studio
winget install Microsoft.VisualStudio.2022.BuildTools

# Instalar Git
winget install Git.Git

# Instalar Python
winget install Python.Python.3.11
```

### 2. Instalar ARM GCC

Descarga e instala ARM GCC desde:
[https://developer.arm.com/downloads/-/gnu-rm](https://developer.arm.com/downloads/-/gnu-rm)

1. Descarga el instalador Windows (.exe)
2. Ejecuta el instalador
3. **Importante**: Marca la opción "Add path to environment variable"

### 3. Clonar el Pico SDK

```powershell
# Crear directorio de desarrollo
cd $env:USERPROFILE
mkdir pico
cd pico

# Clonar SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Configurar variable de entorno (PowerShell)
[Environment]::SetEnvironmentVariable("PICO_SDK_PATH", "$env:USERPROFILE\\pico\\pico-sdk", "User")
```

### 4. Verificar Instalación

Abre una **nueva** ventana de PowerShell:

```powershell
# Verificar CMake
cmake --version

# Verificar ARM GCC
arm-none-eabi-gcc --version

# Verificar SDK
echo $env:PICO_SDK_PATH

# Verificar Git
git --version
```

## Herramientas Opcionales

### picotool

Para programar sin modo BOOTSEL:

```powershell
cd $env:USERPROFILE\pico
git clone https://github.com/raspberrypi/picotool.git
cd picotool
mkdir build
cd build
cmake ..
cmake --build .
```

Agregar a PATH:
```powershell
$picotoolPath = "$env:USERPROFILE\pico\picotool\build"
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$picotoolPath", "User")
```

### Terminal Serial

Opciones para comunicación serial:

1. **PuTTY** (recomendado)
   ```powershell
   winget install PuTTY.PuTTY
   ```

2. **Tera Term**
   ```powershell
   winget install TeraTermProject.teraterm
   ```

3. **Arduino IDE Serial Monitor**

## Compilar un Proyecto

### Usando PowerShell

```powershell
cd mi_proyecto
mkdir build
cd build
cmake -G "NMake Makefiles" ..
nmake
```

### Usando Developer Command Prompt

1. Buscar "Developer Command Prompt for VS 2022"
2. Navegar al proyecto
3. Compilar:

```cmd
cd mi_proyecto
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Usando Visual Studio Code

1. Instalar extensión "CMake Tools"
2. Abrir carpeta del proyecto
3. Presionar `Ctrl+Shift+P`
4. Buscar "CMake: Configure"
5. Seleccionar "ARM GCC" como kit
6. Presionar F7 para compilar

## Cargar el Programa

### Método 1: Modo BOOTSEL

1. Desconecta la placa Pico
2. Mantén presionado el botón BOOTSEL (blanco)
3. Conecta el USB (sin soltar BOOTSEL)
4. Suelta BOOTSEL
5. La placa aparece como "RPI-RP2" en el Explorador de archivos
6. Arrastra el archivo `.uf2` desde `build/` a la unidad RPI-RP2
7. La placa se reinicia automáticamente

### Método 2: Con picotool

```powershell
picotool info
picotool load programa.uf2 -f
picotool reboot
```

## Comunicación Serial

### Encontrar el puerto COM

1. Conectar la Pico
2. Abrir "Administrador de dispositivos"
3. Expandir "Puertos (COM y LPT)"
4. Buscar "USB Serial Device (COMx)"

### Usar PuTTY

1. Abrir PuTTY
2. Seleccionar "Serial"
3. Puerto: COMx (ej. COM3)
4. Velocidad: 115200
5. Clic en "Open"

## Solución de Problemas

### CMake no encuentra el compilador

Verificar que ARM GCC esté en PATH:

```powershell
$env:Path -split ';' | Select-String arm
```

Si no aparece, agregar manualmente:

```powershell
$armPath = "C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\13.2 Rel1\bin"
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$armPath", "User")
```

### PICO_SDK_PATH no encontrado

```powershell
# Verificar
echo $env:PICO_SDK_PATH

# Si está vacío, configurar
[Environment]::SetEnvironmentVariable("PICO_SDK_PATH", "$env:USERPROFILE\\pico\\pico-sdk", "User")

# Reiniciar PowerShell
```

### Error: "ninja: command not found"

Instalar Ninja:

```powershell
winget install Ninja-build.Ninja
```

O usar NMake en su lugar:

```powershell
cmake -G "NMake Makefiles" ..
```

### La Pico no aparece como RPI-RP2

1. Verificar que el cable USB funcione (algunos solo cargan)
2. Intentar con otro puerto USB
3. Mantener BOOTSEL presionado ANTES de conectar
4. Verificar Device Manager para ver si aparece como "RP2 Boot"

### Puerto serial no aparece

Verificar que el programa tenga `pico_enable_stdio_usb` activado:

```cmake
pico_enable_stdio_usb(mi_programa 1)
pico_enable_stdio_uart(mi_programa 0)
```

Y en el código:

```c
#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();  // Importante!
    
    while (1) {
        printf("Hola\n");
        sleep_ms(1000);
    }
}
```

## Configurar VS Code (Opcional)

### Extensiones Recomendadas

1. C/C++ (Microsoft)
2. CMake Tools (Microsoft)
3. Serial Monitor (Microsoft)

### settings.json

```json
{
    "cmake.configureEnvironment": {
        "PICO_SDK_PATH": "C:/Users/TuUsuario/pico/pico-sdk"
    },
    "cmake.generator": "NMake Makefiles"
}
```

## Recursos Adicionales

- [Getting Started with Pico - PDF](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [Pico C/C++ SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico Setup Windows (GitHub)](https://github.com/raspberrypi/pico-setup-windows)
