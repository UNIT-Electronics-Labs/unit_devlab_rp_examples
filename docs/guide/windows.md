# Guía Rápida para Windows 

Esta guía proporciona pasos específicos para configurar DevLab en Windows y evitar problemas comunes.

## Instalación Rápida

### 1. Instalar Requisitos

```powershell
# Instalar 7-Zip (requerido para extraer toolchains)
winget install 7zip.7zip

# Verificar que Python 3.11+ esté instalado
python --version
```

::: tip Descarga Zadig
Descarga Zadig desde [https://zadig.akeo.ie/](https://zadig.akeo.ie/) antes de continuar.
:::

### 2. Configurar Driver USB con Zadig

**Antes de instalar DevLab**, necesitas configurar el driver USB para la FPGA usando Zadig.

1. Descarga Zadig desde [https://zadig.akeo.ie/](https://zadig.akeo.ie/)
2. Conecta tu tarjeta FPGA (Tang Nano 9K u otra) al puerto USB
3. Ejecuta Zadig
4. En el menú **Options**, activa **List All Devices**
5. Selecciona tu dispositivo FPGA de la lista desplegable
   - Para Tang Nano 9K: busca "JTAG Debugger" o "USB-JTAG"
6. Selecciona el driver **WinUSB** o **libusbK** en la lista de la derecha
7. Haz clic en **Replace Driver** o **Install Driver**
8. Espera a que la instalación termine

![Configuración de Zadig](./img/zadig_changew.png)
*Zadig mostrando la instalación exitosa del driver WinUSB para JTAG Debugger*

::: warning Importante
Sin el driver WinUSB/libusbK, openFPGALoader no podrá comunicarse con la FPGA. Este paso es **obligatorio** en Windows.
:::

### 3. Instalar DevLab

```powershell
# Actualizar pip
python -m pip install --upgrade pip

# Instalar DevLab
python -m pip install --upgrade devlab-fpga

# Instalar toolchains
python -m devlab install --force
python -m devlab install-ghdl

# Verificar instalación
python -m devlab --version
python -m devlab doctor
```

## Configuración de Windows Defender (Importante)

**Sin estas exclusiones, la compilación será muy lenta o fallará.**

### Ejecutar PowerShell como Administrador

1. Presiona `Win + X`
2. Selecciona "Windows PowerShell (Administrador)" o "Ejecutar como administrador"

![Ejecutar PowerShell como Administrador](./img/ps_admin.png)
*Menú de Windows mostrando la opción "Ejecutar como administrador"*

3. Copia y pega los siguientes comandos:

```powershell
# Define las rutas de los toolchains
# NOTA: Actualiza las versiones según lo que DevLab instaló
$oss="$env:USERPROFILE\.devlab\toolchains\oss-cad-suite-2026-07-06-windows-x64\oss-cad-suite\bin"
$ghdl="$env:USERPROFILE\.devlab\toolchains\ghdl-6.0.0-windows-x64\bin"

# Agrega exclusiones de procesos
Add-MpPreference -ExclusionProcess "$oss\yosys.exe"
Add-MpPreference -ExclusionProcess "$oss\nextpnr-himbaechel.exe"
Add-MpPreference -ExclusionProcess "$oss\gowin_pack.exe"
Add-MpPreference -ExclusionProcess "$oss\openFPGALoader.exe"
Add-MpPreference -ExclusionProcess "$ghdl\ghdl.exe"

# Agrega exclusión de la carpeta completa (recomendado)
Add-MpPreference -ExclusionPath "$env:USERPROFILE\.devlab"
```

![Comandos de PowerShell](./img/ps_commands.webp)
*Ejecución de los comandos de exclusión en PowerShell como Administrador*

### Verificar las Exclusiones

```powershell
Get-MpPreference | Select-Object -ExpandProperty ExclusionPath
Get-MpPreference | Select-Object -ExpandProperty ExclusionProcess
```

## Uso Diario

### Compilar un Proyecto

```powershell
# Ir al directorio del proyecto
cd blink

# Compilar con Verilog
python -m devlab build

# Compilar con VHDL
python -m devlab build -c devlab-vhdl.toml

# Cargar en la FPGA
python -m devlab flash
```

![Salida de devlab flash](./img/flash.png)
*Resultado exitoso de `devlab flash` cargando el bitstream en la FPGA*

### Comandos Útiles

```powershell
# Ver información del sistema
python -m devlab doctor
```

![Salida de devlab doctor](./img/doctor.png)
*Comando `devlab doctor` mostrando las herramientas instaladas*

```powershell
# Limpiar archivos de compilación
Remove-Item -Recurse -Force build

# Ver ayuda
python -m devlab --help
```

## Solución de Problemas Comunes

### Error: "Command returned non-zero exit status 125"

**Síntoma**: El comando `devlab build` falla con un error de `nextpnr-himbaechel.exe`.

**Causa**: Windows Defender está bloqueando las herramientas.

**Solución**: 
1. Configura las exclusiones de Windows Defender (ver arriba)
2. Reinicia PowerShell
3. Intenta compilar nuevamente

### Error: "7z is not recognized"

**Síntoma**: DevLab no puede extraer los toolchains.

**Causa**: 7-Zip no está instalado o no está en el PATH.

**Solución**:
```powershell
winget install 7zip.7zip
```

Reinicia PowerShell después de la instalación.

### Error: "Could not find JTAG device" al Cargar

**Síntoma**: `devlab flash` no encuentra la FPGA conectada.

**Causa**: Driver USB no configurado correctamente.

**Solución**:
1. Usa Zadig para instalar el driver WinUSB/libusbK (ver sección de instalación)
2. Desconecta y reconecta la FPGA
3. Verifica en el Administrador de Dispositivos que aparezca bajo "Universal Serial Bus devices"
4. Intenta `python -m devlab flash` nuevamente

### Error: "python: command not found"

**Síntoma**: Windows no encuentra Python.

**Causa**: Python no está instalado o no está en el PATH.

**Solución**:
1. Descarga Python desde [python.org](https://www.python.org/downloads/)
2. Durante la instalación, marca "Add Python to PATH"
3. Reinicia PowerShell

### La Compilación es Muy Lenta

**Causa**: Windows Defender está escaneando cada archivo.

**Solución**: Configura las exclusiones de Windows Defender (ver sección de configuración arriba).

### Error: "Access Denied" al Configurar Exclusiones

**Causa**: PowerShell no se ejecutó como Administrador.

**Solución**:
1. Cierra PowerShell
2. Presiona `Win + X`
3. Selecciona "Windows PowerShell (Administrador)"
4. Intenta nuevamente

## Actualizar Versiones de Toolchain

Las rutas en los scripts de exclusión incluyen números de versión. Cuando DevLab se actualice, verifica las rutas reales:

```powershell
# Ver el contenido del directorio de toolchains
ls $env:USERPROFILE\.devlab\toolchains
```

Actualiza las variables `$oss` y `$ghdl` con las nuevas versiones si es necesario.

## Recursos Adicionales

- [Guía General de DevLab](./devlab.md)
- [Archivos CST](./cst.md)
- [Introducción a Verilog](./verilog.md)
- [Introducción a VHDL](./vhdl.md)

## Nota de Seguridad

Las exclusiones de Windows Defender reducen la protección del sistema en las carpetas y procesos excluidos. Solo configura estas exclusiones si:

1. Confías en las herramientas de DevLab
2. Descargaste DevLab desde fuentes oficiales
3. Entiendes los riesgos de seguridad

Considera usar una máquina virtual o WSL2 si prefieres mantener Windows Defender completamente activo.
