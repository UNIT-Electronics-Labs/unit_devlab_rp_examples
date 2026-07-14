# Ejemplo 13: osciloscopio OLED

Osciloscopio sencillo de un canal para Raspberry Pi Pico. Captura 1024 muestras
con ADC, FIFO y DMA a **500 kS/s**. Para graficarlas, promedia cuatro muestras
por cada pixel de una pantalla SSD1306 de 128x64. Una senal de 1 kHz se adquiere
con aproximadamente 500 muestras por periodo.

## Conexiones

| Funcion | Pin Pico |
|---|---|
| Entrada de senal | GP26 / ADC0 |
| OLED SDA | GP0 |
| OLED SCL | GP1 |
| Tierra comun | GND |

La direccion I2C usada es `0x3C`. La entrada ADC debe permanecer entre **0 V y
3.3 V**: no conectes voltajes negativos ni mayores de 3.3 V directamente al
RP2040. Todas las tierras deben estar conectadas.

Para una senoide de generador usa, por ejemplo, **1 Vpp con offset de 1.65 V**.
Una senoide configurada con offset de 0 V tiene su semiciclo negativo fuera del
rango del ADC: se recorta, deja de parecer una senoide y puede perder el
disparo. Si el generador no permite offset, se necesita una etapa de polarizacion
y proteccion antes de GP26.

## Escala de tiempo

`SAMPLE_RATE_HZ` en `main.c` define la frecuencia de muestreo. Su valor inicial
es 500000 muestras/s, el maximo nominal del ADC del RP2040. Cada pixel horizontal
resume cuatro conversiones y representa 8 us; la pantalla completa muestra
1.024 ms. Para cambiar la escala, usa un valor menor o igual a 500000.

El disparo ascendente con histeresis mantiene estable una senal periodica aun
con ruido moderado. La escala vertical se ajusta automaticamente y cambia de
forma gradual para evitar que la grafica salte entre capturas.

## Compilacion

```bash
cd examples/13_oled_oscillocope
picodev build
```

El controlador SSD1306 se reutiliza desde `examples/12_oled_application`.
