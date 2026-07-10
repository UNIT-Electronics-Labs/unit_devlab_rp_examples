# IMU BMI270 con visualización en display OLED (I²C)

## Objetivo

Se presenta una aplicación avanzada que combina dos dispositivos I²C sobre el
mismo bus: una unidad de medición inercial (IMU) BMI270 de seis ejes
(acelerómetro y giroscopio) y un display OLED SSD1306 de 128×64 píxeles. El
microcontrolador RP2040 realiza la lectura periódica del sensor y presenta los
resultados en tiempo real sobre el display, sin depender del monitor serie.

A diferencia del ejemplo básico de comunicación I²C con OLED (sección
Ejemplos Básicos), en el que un único dispositivo ocupa el bus, aquí se
introduce el direccionamiento múltiple sobre el mismo periférico I²C, la
selección de dirección alterna cuando un dispositivo no responde en su
dirección por defecto, y el uso del segundo bloque I²C físico del RP2040
(I2C1) en lugar del bloque por defecto (I2C0).

## Descripción general

| Dispositivo | Función | Dirección I²C |
|---|---|---|
| BMI270 | IMU de 6 ejes (acelerómetro + giroscopio) | `0x68` (primaria) o `0x69` (secundaria) |
| SSD1306 | Display OLED 128×64 | `0x3C` (usual) o `0x3D` (alterna) |

Ambos dispositivos comparten físicamente las mismas líneas SDA y SCL. Esto es
posible porque el protocolo I²C direcciona cada transacción a un dispositivo
específico mediante los 7 bits de dirección que preceden a cada trama; ambos
periféricos pueden coexistir en el mismo bus siempre que sus direcciones no
coincidan.

## Hardware requerido

- Placa RP2040 (modelo AR3203, USB-C)
- Módulo IMU BMI270
- Display OLED SSD1306 128×64
- Cables de conexión / protoboard

## Conexiones

| Señal | BMI270 | SSD1306 | GPIO RP2040 |
|---|---|---|---|
| SDA | SDA | SDA | GP2 |
| SCL | SCL | SCL | GP3 |
| VCC | VCC | VCC | 3V3 |
| GND | GND | GND | GND |

Ambos dispositivos se conectan en paralelo sobre el mismo par de líneas.

## Selección del periférico I²C: por qué `Wire1`

El RP2040 cuenta con **dos bloques de hardware I²C independientes** (I2C0 e
I2C1), cada uno accesible mediante GPIOs específicos.

| GPIO SDA | GPIO_SCL | Bloque I²C |
|---|---|---|
| 0 | 1 | I2C0 |
| 2 | 3 | I2C1 |
| 4 | 5 | I2C0 |
| 6 | 7 | I2C1 |
| 8 | 9 | I2C0 |
| 10 | 11 | I2C1 |
| 12 | 13 | I2C0 |
| 14 | 15 | I2C0 |
| 16 | 17 | I2C0 |
| 18 | 19 | I2C1 |
| 20 | 21 | I2C0 |
| 26 | 27 | I2C1 |

::: info
(Para referencia visual. Consultar 
la tabla completa de mapeo GPIO↔periférico en [hardware.md](../guide/hardware.md#pinout-completo).)
:::

En el core RP2040 para Arduino, el objeto `Wire` está enlazado en tiempo de
compilación al bloque I2C0, y el objeto `Wire1` al bloque I2C1. Los métodos
`setSDA()` y `setSCL()` **no reasignan el periférico**: únicamente indican qué
pines usará el bloque ya fijo de ese objeto, y validan que el pin pertenezca
al bloque correspondiente.

Dado que este ejemplo usa GP2/GP3 (que corresponden a I2C1), el código debe
usar `Wire1` en lugar de `Wire` — tanto para inicializar el bus como para el
constructor del objeto `display`. Este es el motivo por el que este ejemplo
difiere del ejemplo básico de I²C, que usa el bloque I2C0 por defecto.

## Bibliotecas requeridas (Instalación)

Instalables desde el Gestor de Bibliotecas del Arduino IDE:

- **Adafruit SSD1306** (requiere como dependencia **Adafruit GFX Library**)
- **SparkFun BMI270 Arduino Library**

## Estructura del proyecto

```
i2c_oled/
└── i2c_oled.ino
```

## Código completo

```cpp
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "SparkFun_BMI270_Arduino_Library.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SDA_PIN 2
#define SCL_PIN 3

BMI270 imu;
// I2C address selection
uint8_t addr_imu_1 = BMI2_I2C_PRIM_ADDR;  // 0x68
uint8_t addr_imu_2 = BMI2_I2C_SEC_ADDR; 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Serial.println("BMI270 Example I2C - Basic Readings I2C with OLED print");
  Wire1.setSDA(SDA_PIN);
  Wire1.setSCL(SCL_PIN);
  Wire1.begin();
  Wire1.setClock(400000);

  bool imu_ok = false;
  Serial.println("Inicializando BMI270...");

  if (imu.beginI2C(addr_imu_1, Wire1) == BMI2_OK) {
    Serial.println("BMI270 detectado en 0x68");
    imu_ok = true;
  } else if (imu.beginI2C(addr_imu_2, Wire1) == BMI2_OK) {
    Serial.println("BMI270 detectado en 0x69");
    imu_ok = true;
  }
  while (!imu_ok) {
    Serial.println("Error: BMI270 no detectado en 0x68 ni 0x69");
    delay(1000);
  }

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
  display.println("BMI270 Test");
  display.display();
}

void loop() {
  imu.getSensorData();

  // Serial (debug completo)
  Serial.print("Acceleration in g's\t");
  Serial.print("X: "); Serial.print(imu.data.accelX, 3); Serial.print("\t");
  Serial.print("Y: "); Serial.print(imu.data.accelY, 3); Serial.print("\t");
  Serial.print("Z: "); Serial.println(imu.data.accelZ, 3);

  Serial.print("Rotation in deg/sec\t");
  Serial.print("X: "); Serial.print(imu.data.gyroX, 3); Serial.print("\t");
  Serial.print("Y: "); Serial.print(imu.data.gyroY, 3); Serial.print("\t");
  Serial.print("Z: "); Serial.println(imu.data.gyroZ, 3);

  // OLED
  display.clearDisplay();

  // Título grande arriba
  display.setTextSize(2);
  display.setCursor(28, 0);
  display.println("BMI270");

  // Línea separadora bajo el título
  display.drawFastHLine(0, 18, SCREEN_WIDTH, SSD1306_WHITE);

  // Volver a tamaño normal para los datos
  display.setTextSize(1);

  // Encabezados de columna
  display.setCursor(0, 22);
  display.print("Accel");
  display.setCursor(70, 22);
  display.print("Gyro");

  // Columna izquierda: aceleración
  display.setCursor(0, 32);
  display.print("X:"); display.println(imu.data.accelX, 2);
  display.setCursor(0, 42);
  display.print("Y:"); display.println(imu.data.accelY, 2);
  display.setCursor(0, 52);
  display.print("Z:"); display.println(imu.data.accelZ, 2);

  // Columna derecha: giroscopio
  display.setCursor(70, 32);
  display.print("X:"); display.println(imu.data.gyroX, 2);
  display.setCursor(70, 42);
  display.print("Y:"); display.println(imu.data.gyroY, 2);
  display.setCursor(70, 52);
  display.print("Z:"); display.println(imu.data.gyroZ, 2);

  display.display();

  delay(20);
}
```

## Explicación del funcionamiento

### Inicialización (`setup()`)

1. Se configura el puerto serie a 115200 baudios, usado únicamente para
   depuración.
2. Se asignan los pines GP2/GP3 al objeto `Wire1` y se inicializa el bus a
   400 kHz (modo I²C Fast Mode).
3. Se intenta inicializar el BMI270 primero en su dirección primaria
   (`0x68`); si no responde, se reintenta en la dirección secundaria
   (`0x69`). Ambas direcciones son seleccionables físicamente en la mayoría
   de los módulos BMI270 mediante el pin `SDO`/`ADDR`.
4. Se inicializa el display, con el mismo patrón de dirección alterna
   (`0x3C` / `0x3D`).
5. Se escribe un mensaje inicial de verificación ("BMI270 Test") y se llama a
   `display.display()` para confirmar que el display responde antes de
   entrar al ciclo principal.

### Ciclo principal (`loop()`)

En cada iteración:

1. `imu.getSensorData()` actualiza la estructura `imu.data` con las lecturas
   más recientes de acelerómetro (`accelX/Y/Z`, en g) y giroscopio
   (`gyroX/Y/Z`, en grados/segundo).
2. Los seis valores se imprimen por el puerto serie para depuración.
3. El buffer del display se limpia (`clearDisplay()`) y se reconstruye por
   completo en cada ciclo: título, línea separadora, encabezados de columna
   y los seis valores organizados en dos columnas (aceleración e izquierda,
   giroscopio a la derecha).
4. `display.display()` transmite el buffer completo al controlador SSD1306
   por I²C. Este paso es indispensable: toda la API de Adafruit_GFX
   (`print`, `println`, `drawFastHLine`, etc.) solo escribe sobre un buffer
   en RAM, no sobre la pantalla física.
5. `delay(20)` limita la tasa de refresco a aproximadamente 50 Hz.

## Compilación y carga

1. En el Arduino IDE, seleccionar la placa RP2040 correspondiente y el
   puerto correcto en el menú **Herramientas**.
2. Si es la primera carga sobre la placa (o el bootloader UF2 no tiene aún
   firmware Arduino), debe entrarse en modo **BOOTSEL**: desconectar el USB,
   mantener presionado el botón BOOTSEL,conectar el USB, presionar el boton RST
   manteniendo el boton BOOTSEL presionado y soltar el botón
   un par de segundos después. La placa aparece como unidad de
   almacenamiento masivo y, en el IDE, como un puerto especial de tipo UF2.
3. En cargas posteriores, el propio botón "Subir" reinicia la placa a modo
   bootloader automáticamente; no debería ser necesario repetir el paso 2.
4. En Linux, si el puerto no aparece o se reporta error de permisos,
   verificar que el usuario pertenezca al grupo `dialout`:
   ```bash
   sudo usermod -aG dialout $USER
   ```
   y reiniciar sesión para que el cambio surta efecto.

## Verificación

Al finalizar la carga, el display debe mostrar el encabezado "BMI270", una
línea separadora, y dos columnas ("Accel" / "Gyro") con los valores de X, Y y
Z actualizándose en tiempo real conforme se mueve la placa. El monitor serie
(115200 baudios) muestra en paralelo las mismas seis lecturas con mayor
precisión (3 decimales frente a los 2 mostrados en el OLED, por restricción
de espacio horizontal).

## Variantes y extensiones posibles

- Incorporar la lectura de temperatura interna del BMI270 (`imu.data.temperature`, si la biblioteca la expone) como tercer bloque de datos.
- Sustituir el `delay(20)` por temporización basada en `millis()` para permitir tareas adicionales sin bloquear el ciclo.
- Aplicar un filtro complementario simple entre acelerómetro y giroscopio como introducción a la estimación de orientación (ángulos de cabeceo y alabeo).
- Sustituir la impresión por puerto serie por el Serial Plotter del Arduino IDE para visualización gráfica en tiempo real.
- Publicar las lecturas por Wi-Fi (enlace natural con el Módulo III del seminario, que cubre conectividad IoT).

## Errores comunes

| Síntoma | Causa | Solución |
|---|---|---|
| `no matching function for call to 'TwoWire::begin(int, int, int)'` | La sobrecarga de 3 argumentos de `Wire.begin(sda, scl, freq)` pertenece al core de ESP32, no al de RP2040. | Usar `setSDA()` / `setSCL()` / `begin()` / `setClock()` por separado. |
| El sensor o el display no responden aunque el código compila | Se usó `Wire` en vez de `Wire1` (o viceversa) para pines que pertenecen al otro bloque I²C. | Confirmar con la tabla de mapeo GPIO↔bloque I²C cuál objeto corresponde a los pines usados. |
| La pantalla se queda congelada mostrando el primer mensaje | Falta la llamada a `display.display()` dentro de `loop()`. Toda la API gráfica solo modifica un buffer en RAM. | Llamar a `display.display()` al final de cada ciclo. |
| Los datos nuevos se superponen a los anteriores en el OLED | Falta `display.clearDisplay()` antes de redibujar el buffer. | Limpiar el buffer al inicio de cada actualización. |
| El IDE no detecta la placa o no sube el sketch | La placa no está en modo bootloader (primera carga), o el usuario no tiene permisos sobre el puerto serie en Linux. | Entrar en modo BOOTSEL manualmente; en Linux, agregar el usuario al grupo `dialout`. |

Se observa además que el bloque `while (!imu_ok) { ... delay(1000); }` de la
función `setup()` no reintenta la detección del sensor: si ambas direcciones
fallan en el primer intento, el programa queda indefinidamente imprimiendo el
mensaje de error sin volver a intentar `beginI2C()`. Para un comportamiento
robusto ante fallas transitorias de alimentación o conexión, ese bloque
debería repetir también las llamadas a `imu.beginI2C()` dentro del ciclo, no
solo el mensaje de error.
