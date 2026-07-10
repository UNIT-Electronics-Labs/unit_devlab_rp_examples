# Visualización del TMP235 en Pantalla OLED (Arduino)

Extiende la práctica **Lectura analógica del TMP235 mediante ADC (Arduino)** agregando una pantalla OLED SSD1306 por I²C para mostrar los valores en tiempo real, sin depender del monitor serie.

## Objetivo

Leer el sensor TMP235 mediante el ADC y mostrar en una pantalla OLED SSD1306 (128×64, I²C) los valores crudos del ADC, el voltaje calculado y la temperatura resultante.

## Hardware

| Componente | Cantidad |
|---|---|
| Placa RP2040 compatible con Arduino | 1 |
| Sensor TMP235 (empaque SOT-23) | 1 |
| Pantalla OLED SSD1306 128×64 (I²C) | 1 |
| Cables de conexión | 7 |

## Conexiones

**TMP235** (ver también [Lectura analógica del TMP235 mediante ADC](./lectura-analogica-tmp235-arduino.md)):

| Pin del TMP235 | Señal | Conexión en la placa |
|---|---|---|
| 1 | VDD | 3V3 |
| 2 | VOUT | GPIO26 (A0) |
| 3 | GND | GND |

**Pantalla OLED SSD1306:**

| Señal | Pin | Notas |
|---|---|---|
| SDA | GPIO2 | Requiere `Wire1` (ver explicación) |
| SCL | GPIO3 | Requiere `Wire1` |
| VCC | 3.3V | |
| GND | GND | |

## Por qué Wire1 y no Wire

En el core arduino-pico, `Wire` está enlazado al periférico I2C0 y `Wire1` al I2C1. Los pines GPIO2/GPIO3 corresponden a I2C1 según el mapeo de periféricos del RP2040, por lo que deben inicializarse con `Wire1`, no con `Wire`.

## Librerías requeridas

- `Wire` (incluida en el core)
- `Adafruit_SSD1306` (y su dependencia `Adafruit_GFX`)

## Código completo

```cpp
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define TMP235_PIN A0
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SDA_PIN 2
#define SCL_PIN 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Serial.println("ADC TMP235");
  analogReadResolution(12);   // Resolucion nativa del ADC; el core usa 10 bits por defecto
  Wire1.setSDA(SDA_PIN);
  Wire1.setSCL(SCL_PIN);
  Wire1.begin();
  Wire1.setClock(400000);
  Serial.println("Inicializando TMP235...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error con 0x3C, probando 0x3D...");
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println("Error OLED en ambas direcciones");
      return;
    }
  }
  Serial.println("OLED inicializada!");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("TMP235 Test");
  display.display();
}

void loop(){
  int raw = analogRead(TMP235_PIN);        // 0 a 4095
  float voltage = raw * 3.3 / 4096.0;      // Voltaje real
  float tempC = (voltage - 0.5) * 100.0;   // Fórmula TMP235

  display.clearDisplay();

  // Título grande arriba
  display.setTextSize(2);
  display.setCursor(28, 0);
  display.println("TMP235");
  display.drawFastHLine(0, 18, SCREEN_WIDTH, SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 22);
  display.print("Raw");
  display.setCursor(70, 22);
  display.print("Voltage");
  display.setCursor(25, 42);
  display.print("Temperature");

  display.setCursor(0, 32);
  display.print("raw=");
  display.print(raw);
  Serial.print("raw=");
  Serial.print(raw);

  display.setCursor(70, 32);
  display.print("V=");
  display.print(voltage,2);
  Serial.print("  V=");
  Serial.print(voltage, 3);

  display.setCursor(25, 52);
  display.print("T=");
  display.print(tempC,2);
  Serial.print("  T=");
  Serial.print(tempC, 2);
  Serial.println(" C");

  display.display();

  delay(20);
}
```

> Nota editorial: el código original incluía `#include "SparkFun_BMI270_Arduino_Library.h"`, heredado de una práctica previa con IMU. Al no usarse ningún objeto ni función de esa librería en esta aplicación, se retiró del listado para no introducir una dependencia innecesaria en el material del curso.

## Explicación

`analogRead()` sobre `A0` (GPIO26) entrega el valor crudo del ADC en la resolución configurada. El core arduino-pico usa 10 bits (0-1023) por defecto para mantener compatibilidad con el Arduino clásico, mientras que la conversión a voltaje (`raw * 3.3 / 4096.0`) asume 12 bits (0-4095). Por eso `analogReadResolution(12)` se llama explícitamente en `setup()`: sin ella, el voltaje y la temperatura calculados saldrían subestimados en un factor de aproximadamente 4.

La pantalla se organiza en dos filas: la primera muestra "Raw" y "Voltage" lado a lado (columnas en x=0 y x=70), la segunda muestra "Temperature" centrada debajo. Cada `display.clearDisplay()` borra el framebuffer completo antes de redibujar, y `display.display()` al final del ciclo transmite ese framebuffer por I²C hacia la pantalla.

## Verificación

Al iniciar debe observarse brevemente el mensaje "TMP235 Test"; a partir de la primera iteración de `loop()`, la pantalla debe mostrar el título "TMP235", la línea separadora, y los tres valores actualizándose de forma continua.

## Errores comunes

| Síntoma | Causa típica |
|---|---|
| "Error OLED en ambas direcciones" | Cableado de SDA/SCL invertido, o falta de alimentación en la pantalla |
| No compila la referencia a `Wire1` | La placa seleccionada en el IDE no corresponde a un core con dos periféricos I2C disponibles |
| Parpadeo visible en la pantalla | El ciclo se refresca cada 20 ms; considerar aumentar el `delay()` (ver Variantes) |

## Variantes

- Aumentar el intervalo de refresco (por ejemplo, 250-500 ms), dado que la temperatura no cambia a la velocidad del ciclo actual (`delay(20)`); esto reduce el tráfico I²C y el parpadeo visual.
- Sustituir los valores numéricos por una representación gráfica simple (barra o tendencia).
