# Ejemplos y Prácticas para RP2040

Repositorio de ejemplos y documentación para aprender programación de microcontroladores usando el **RP2040** (Raspberry Pi Pico) en **C** y **ensamblador ARM**.

## 📚 Contenido Temático

- Arquitecturas de microcontroladores de propósito general
- Implementación utilizando RP2040 como plataforma compatible con arquitectura Cortex-M0+
- Programación en lenguaje ensamblador
- Programación en lenguaje C
- Entradas y salidas digitales
- Comunicación UART
- Comunicación I²C
- Comunicación SPI
- ADC y temporizadores
- Herramientas de desarrollo para microcontroladores

## 🔬 Prácticas de Laboratorio

1. **[Lectura de entradas digitales](docs/examples/gpio.md)** - GPIO, botones, LEDs, interrupciones
2. **[Comunicación UART](docs/examples/uart.md)** - Serial, comandos, shell interactivo
3. **[Lectura de temperatura mediante ADC](docs/examples/adc.md)** - Sensor interno, muestreo, DMA
4. **[Comunicación I²C con display OLED](docs/examples/i2c-oled.md)** - SSD1306, gráficos, animaciones

## 🚀 Inicio Rápido

### Requisitos

- Placa con RP2040 (Raspberry Pi Pico, Pico W, etc.)
- CMake 3.13+
- Compilador ARM GCC (arm-none-eabi-gcc)
- Pico SDK
- Cable USB

### Instalación

#### Linux (Ubuntu/Debian)

```bash
# Instalar herramientas
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi \
  build-essential libstdc++-arm-none-eabi-newlib git

# Clonar Pico SDK
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Configurar variable de entorno
echo 'export PICO_SDK_PATH=$HOME/pico-sdk' >> ~/.bashrc
source ~/.bashrc
```

#### Windows

Ver [Guía completa para Windows](docs/guide/windows.md)

#### macOS

```bash
brew install cmake
brew tap ArmMbed/homebrew-formulae
brew install arm-none-eabi-gcc

cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

echo 'export PICO_SDK_PATH=$HOME/pico-sdk' >> ~/.zshrc
source ~/.zshrc
```

### Compilar un Ejemplo

```bash
cd example
mkdir build && cd build
cmake ..
make
```

### Cargar a la Placa

1. Mantén presionado el botón **BOOTSEL**
2. Conecta el USB (sin soltar BOOTSEL)
3. Suelta BOOTSEL
4. Copia el archivo `.uf2` a la unidad que aparece

## 📖 Documentación

La documentación completa está disponible en el directorio `docs/`:

- [Configuración del SDK](docs/guide/devlab.md)
- [Guía para Windows](docs/guide/windows.md)
- [Programación en C](docs/guide/c-programming.md)
- [Programación en Ensamblador](docs/guide/asm-programming.md)
- [Índice de ejemplos](docs/examples/index.md)

### Documentación Web (VitePress)

```bash
cd docs
npm install
npm run docs:dev
```

Visita http://localhost:5173 para ver la documentación interactiva.

## 📁 Estructura del Proyecto

```
.
├── docs/              # Documentación VitePress
│   ├── examples/      # Guías de prácticas
│   │   ├── gpio.md
│   │   ├── uart.md
│   │   ├── adc.md
│   │   └── i2c-oled.md
│   └── guide/         # Guías de configuración
│       ├── devlab.md
│       ├── windows.md
│       ├── c-programming.md
│       └── asm-programming.md
└── example/           # Código de ejemplo
```

## 🛠️ Herramientas Recomendadas

- **CMake** - Sistema de build
- **VS Code** - Editor con extensiones C/C++ y CMake Tools
- **picotool** - Herramienta para programar sin modo BOOTSEL
- **minicom/screen** - Terminal serial (Linux/macOS)
- **PuTTY** - Terminal serial (Windows)

## 📚 Recursos Adicionales

- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
- [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [Raspberry Pi Pico C/C++ SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)

## 📝 Licencia

Este proyecto es material educativo de código abierto.

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor, abre un issue o pull request.

## 📧 Soporte

Para preguntas o problemas, abre un issue en el repositorio.
