# Usar DevLab

DevLab es el flujo principal de este repositorio para compilar y cargar ejemplos FPGA sin depender de `make`. Cada ejemplo contiene un archivo `devlab.toml` y, cuando aplica, un `devlab-vhdl.toml`.

## Requisitos

- Node.js 22+ para la documentación VitePress.
- Python 3.11+ para DevLab.
- `devlab-fpga` instalado.
- OSS CAD Suite instalado desde DevLab.
- Una tarjeta con FPGA Gowin compatible conectada por USB para cargar el bitstream.
- **Windows**: 7-Zip instalado para extraer toolchains.

## Instalar DevLab

```bash
pip install devlab-fpga
devlab install
```

## Configuración Adicional para Windows

Windows Defender puede interferir con las herramientas de síntesis y ralentizar el proceso de compilación. Se recomienda configurar exclusiones antes de instalar DevLab.

::: tip Guía Detallada para Windows
Para instrucciones completas con capturas de pantalla, consulta la [Guía para Windows](./windows.md).
:::

### Configurar Driver USB con Zadig (Obligatorio)

Para programar la FPGA en Windows, necesitas instalar el driver USB correcto:

1. Descarga Zadig desde [https://zadig.akeo.ie/](https://zadig.akeo.ie/)
2. Conecta tu FPGA (Tang Nano 9K u otra)
3. Ejecuta Zadig y activa **Options → List All Devices**
4. Selecciona tu dispositivo FPGA ("JTAG Debugger" o "USB-JTAG")
5. Selecciona **WinUSB** o **libusbK** como driver
6. Haz clic en **Replace Driver** o **Install Driver**

::: warning
Sin este driver, `devlab flash` no funcionará. Este paso es obligatorio.
:::

### Instalar 7-Zip

DevLab requiere 7-Zip para extraer los toolchains:

```powershell
winget install 7zip.7zip
```

### Instalar DevLab con Python

En Windows, usa el módulo Python explícitamente:

```powershell
python -m pip install --upgrade devlab-fpga
python -m devlab install --force
python -m devlab install-ghdl
```

### Configurar Exclusiones de Windows Defender

Ejecuta PowerShell como **Administrador** y agrega las siguientes exclusiones para evitar que Windows Defender escanee los ejecutables de las herramientas FPGA:

```powershell
# Define las rutas de los toolchains
$oss="$env:USERPROFILE\.devlab\toolchains\oss-cad-suite-2026-07-06-windows-x64\oss-cad-suite\bin"
$ghdl="$env:USERPROFILE\.devlab\toolchains\ghdl-6.0.0-windows-x64\bin"

# Agrega exclusiones de procesos
Add-MpPreference -ExclusionProcess "$oss\yosys.exe"
Add-MpPreference -ExclusionProcess "$oss\nextpnr-himbaechel.exe"
Add-MpPreference -ExclusionProcess "$oss\gowin_pack.exe"
Add-MpPreference -ExclusionProcess "$oss\openFPGALoader.exe"
Add-MpPreference -ExclusionProcess "$ghdl\ghdl.exe"

# Agrega exclusión de la carpeta completa
Add-MpPreference -ExclusionPath "$env:USERPROFILE\.devlab"
```

::: warning Nota de Seguridad
Las exclusiones de Windows Defender reducen la protección del sistema. Solo agrega estas exclusiones si confías en las herramientas de DevLab.
:::

### Verificar la Instalación

```powershell
python -m devlab --version
python -m devlab doctor
```

### Usar DevLab en Windows

Una vez instalado, usa `python -m devlab` en lugar de solo `devlab`:

```powershell
python -m devlab build
python -m devlab flash
```

## Compilar Verilog

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
