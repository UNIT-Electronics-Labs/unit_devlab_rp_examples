# Guía para Windows - RP2040

Esta guía proporciona pasos específicos para configurar el entorno de desarrollo
RP2040 en Windows usando `picodev`, el CLI que automatiza la instalación del
Pico SDK y el toolchain ARM GCC.

## Instalación Rápida

### 1. Instalar CMake

`picodev` no instala CMake; es un requisito previo que se instala de forma manual.

Puede descargar el instalador desde https://cmake.org/download/ y
asegurarse de seleccionar la opción **"Add CMake to the system PATH"** durante
la instalación.

Abrir una **nueva** ventana de PowerShell y verificar:

```powershell
cmake --version
```

### 2. Instalar Python

Si Python no está instalado:

```powershell
winget install Python.Python.3.14
```

### 3. Instalar picodev

```powershell
pip install picodev
```

### 4. Instalar el Pico SDK y el toolchain ARM GCC

```powershell
picodev install
```

Este comando descarga el Pico SDK y el toolchain ARM GCC correspondientes a
Windows x64, además de Ninja y picotool, en `~/.picodev`. No requiere
configurar variables de entorno manualmente.

### 5. Verificar la instalación

Abrir una **nueva** ventana de PowerShell:

```powershell
picodev doctor
```

`picodev doctor` verifica que CMake, el Pico SDK y el toolchain ARM GCC estén
disponibles y correctamente configurados.

## Crear y Compilar un Proyecto

```powershell
picodev new blink
cd blink
picodev build
```

La salida se genera en `build/blink.uf2`. Para limpiar los archivos de
compilación generados:

```powershell
picodev clean
```

El CLI también puede invocarse a través del intérprete de Python:

```powershell
python -m picodev --help
```

## Cargar el Programa

### Método recomendado: SWD con picodev flash

1. Conectar una sonda de depuración compatible con CMSIS-DAP a los pines SWD de la placa
2. Conectar la sonda por USB
3. Ejecutar:

```powershell
picodev flash
```

`picodev flash` programa el firmware ELF por SWD usando pyOCD, seleccionando
automáticamente `rp2040` o `rp2350` según la placa. Para listar las sondas
detectadas:

```powershell
picodev flash --detect
```

Usar `--probe <ID>` cuando hay varias sondas conectadas.

### Alternativa: Modo BOOTSEL

Este método no requiere sonda de depuración, pero no forma parte del flujo
principal del curso.

1. Desconecta la placa Pico
2. Mantén presionado el botón BOOTSEL (blanco)
3. Conecta el USB (sin soltar BOOTSEL)
4. Suelta BOOTSEL
5. La placa aparece como "RPI-RP2" en el Explorador de archivos
6. Arrastra el archivo `.uf2` desde `build/` a la unidad RPI-RP2
7. La placa se reinicia automáticamente

## Comunicación Serial

### Encontrar el puerto COM

1. Conectar la placa
2. Abrir "Administrador de dispositivos"
3. Expandir "Puertos (COM y LPT)"
4. Buscar "USB Serial Device (COMx)"

### Opciones de terminal serial

1. **PuTTY** (recomendado)
    Puede descargar el instalador desde https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html

   Abrir PuTTY, seleccionar "Serial", indicar el puerto (ej. COM3) y
   velocidad 115200, luego clic en "Open".

2. **Tera Term**

    Puede descargar eñ instalador desde el repositorio original de TeraTerm usando el ultimo instalador lanzado
    https://github-com.translate.goog/TeraTermProject/teraterm/releases?_x_tr_sl=en&_x_tr_tl=es&_x_tr_hl=es&_x_tr_pto=tc

## Solución de Problemas

### `picodev doctor` reporta CMake faltante

Verificar que CMake esté en PATH:

```powershell
$env:Path -split ';' | Select-String cmake
```

Si no aparece, reinstalar con `winget install --exact --id Kitware.CMake --source winget`
asegurando la opción de agregar a PATH, o agregarlo manualmente y reiniciar PowerShell.

### `picodev doctor` reporta SDK o toolchain faltante

Volver a ejecutar la instalación:

```powershell
picodev install
```

Si se está usando una ubicación personalizada, verificar las variables de
entorno `PICO_SDK_PATH` y `PICO_TOOLCHAIN_PATH`.

### `picodev.exe` no se reconoce como comando

Esto ocurre cuando el directorio de Scripts de Python no está en el PATH de
usuario. La forma modular sigue disponible sin modificar el PATH:

```powershell
python -m picodev doctor
python -m picodev install
```

Para usar el comando corto `picodev`, imprimir el directorio de Scripts y
agregarlo al PATH de usuario:

```powershell
python -c "import sysconfig; print(sysconfig.get_path('scripts'))"
```

### `picodev flash` no detecta la sonda

1. Verificar que la sonda CMSIS-DAP esté conectada por USB
2. Ejecutar `picodev flash --detect` para listar las sondas disponibles
3. Si hay varias sondas conectadas, especificar `--probe <ID>`

### La placa no aparece como RPI-RP2 (modo BOOTSEL)

1. Verificar que el cable USB funcione (algunos solo cargan)
2. Intentar con otro puerto USB
3. Mantener BOOTSEL presionado ANTES de conectar
4. Verificar el Administrador de dispositivos para ver si aparece como "RP2 Boot"

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

## Recursos Adicionales

- [Getting Started with Pico - PDF](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [Pico C/C++ SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
