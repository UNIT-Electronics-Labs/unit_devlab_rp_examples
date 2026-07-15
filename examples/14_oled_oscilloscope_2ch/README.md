# Ejemplo 14: osciloscopio OLED de dos canales

Osciloscopio sencillo de dos canales para Raspberry Pi Pico. Como el RP2040
tiene un solo convertidor ADC, captura primero ADC0 y despues ADC1 mediante FIFO
y DMA, a **500 kS/s por canal**. Descarta las primeras conversiones despues de
cambiar de entrada para evitar residuos del multiplexor. CH1 se dibuja en la
mitad superior de la SSD1306 de 128x64 y CH2 en la mitad inferior.

## Conexiones

| Funcion | Pin Pico |
|---|---|
| Entrada CH1 (arriba y disparo) | GP26 / ADC0 |
| Entrada CH2 (abajo) | GP27 / ADC1 |
| OLED SDA | GP0 |
| OLED SCL | GP1 |
| Tierra comun | GND |

Para medir un filtro, conecta **CH1 antes del pasabajas** y **CH2 despues del
pasabajas**. La pantalla muestra `ATT xx%`, calculado con las amplitudes pico a
pico: `100 * Vpp_CH2 / Vpp_CH1`.

La direccion I2C usada es `0x3C`. La entrada ADC debe permanecer entre **0 V y
3.3 V**: no conectes voltajes negativos ni mayores de 3.3 V directamente al
RP2040. Todas las tierras deben estar conectadas.

No dejes una entrada desconectada: un ADC flotante puede captar ruido o una copia
de la senal vecina. Para comprobar la separacion, conecta GP27 directamente a GND;
CH2 debe mostrarse casi plana mientras aplicas la senal solamente a GP26. Usa una
fuente de baja impedancia o un buffer para cada entrada.

Para una senoide de generador usa, por ejemplo, **1 Vpp con offset de 1.65 V**.
Una senoide configurada con offset de 0 V tiene su semiciclo negativo fuera del
rango del ADC: se recorta, deja de parecer una senoide y puede perder el
disparo. Si el generador no permite offset, se necesita una etapa de polarizacion
y proteccion antes de GP26 y GP27.

## Escala de tiempo

`ADC_SAMPLE_RATE_HZ` en `main.c` define la frecuencia de cada bloque. Su valor
inicial es 500000 muestras/s, el maximo nominal del ADC del RP2040. Cada pixel
horizontal resume cuatro muestras y representa 8 us; la pantalla completa
muestra 1.024 ms. No uses un valor mayor a 500000.

Cada canal usa su propio disparo ascendente con histeresis. Ambos usan la misma
sensibilidad vertical, aunque cada traza se centra en su propio nivel DC; por
eso la diferencia de amplitud muestra visualmente la atenuacion. Las capturas no
son simultaneas, por lo que no sirven para medir con precision la fase.

Para un pasabajas de primer orden con frecuencia de corte cercana a 1 kHz, los
valores esperados son aproximadamente 89 % a 500 Hz, 71 % a 1 kHz, 45 % a
2 kHz, 32 % a 3 kHz y 24 % a 4 kHz.

## Compilacion

```bash
cd examples/14_oled_oscilloscope_2ch
picodev build
```

El controlador SSD1306 se reutiliza desde `examples/12_oled_application`.
