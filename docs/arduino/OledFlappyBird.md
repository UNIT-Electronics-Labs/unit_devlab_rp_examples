# Ejemplo integrador: juego sobre pantalla OLED

## Descripción general

Este ejemplo implementa un juego del tipo *Flappy Bird* sobre el microcontrolador RP2040, utilizando una pantalla OLED SSD1306 como dispositivo de salida y un sensor táctil capacitivo como dispositivo de entrada. El programa integra en una sola aplicación los conceptos tratados en los ejemplos básicos previos: entradas digitales, comunicación I²C, gráficos sobre OLED y estructuración del programa mediante una máquina de estados.

El código constituye además un caso de estudio de **portabilidad entre plataformas**: el programa original fue desarrollado para ESP32 y se adaptó al RP2040 mediante el núcleo *arduino-pico*. Las modificaciones necesarias para la migración se documentan en una sección específica, dado su valor formativo.

## Objetivos de aprendizaje

- Integrar comunicación I²C, entradas digitales y lógica de aplicación en un programa completo.
- Estructurar una aplicación interactiva mediante una máquina de estados finitos.
- Interpretar la polaridad lógica de un dispositivo de entrada (activo en alto frente a activo en bajo) y sus consecuencias en el código.
- Identificar las diferencias de configuración entre plataformas al portar código de ESP32 a RP2040.
- Emplear un *framebuffer* en RAM y sincronizarlo con la pantalla mediante I²C.

## Material requerido

| Componente | Descripción |
|---|---|
| Tarjeta con RP2040 | Doble núcleo ARM Cortex-M0+, programada mediante el núcleo *arduino-pico* |
| Pantalla OLED SSD1306 | 128 × 64 píxeles, interfaz I²C, dirección `0x3C` |
| Sensor táctil capacitivo | Tipo TTP223 o equivalente; salida digital activa en alto |
| Cables de conexión | Según la tabla de conexiones |

## Conexiones

| Señal | GPIO del RP2040 | Dispositivo |
|---|---|---|
| SDA | GP2 | OLED SSD1306 |
| SCL | GP3 | OLED SSD1306 |
| Salida del sensor | GP7 | Sensor táctil |
| VCC / GND | 3.3 V / GND | Ambos dispositivos |

Los GPIO 2 y 3 corresponden al periférico **I2C1** del RP2040. Esta asignación determina el objeto de comunicación utilizado en el código (`Wire1`), como se explica más adelante.

::: tip Nota sobre el sensor táctil
El sensor capacitivo entrega una salida digital ya acondicionada: en reposo el pin permanece en nivel bajo y, al detectar contacto, conmuta a nivel alto. Desde el punto de vista del programa se comporta como un botón **activo en alto**, sin requerir componentes externos de acondicionamiento.
:::

## Bibliotecas utilizadas

| Biblioteca | Función |
|---|---|
| `Wire.h` | Comunicación I²C (incluida en el núcleo *arduino-pico*) |
| `Adafruit_GFX.h` | Primitivas gráficas: texto, rectángulos, mapas de bits |
| `Adafruit_SSD1306.h` | Controlador de la pantalla OLED; gestiona el *framebuffer* y su envío por I²C |

Las bibliotecas de Adafruit se instalan desde el gestor de bibliotecas del IDE de Arduino. La instalación de `Adafruit SSD1306` incorpora automáticamente `Adafruit GFX Library` como dependencia.

## Arquitectura del programa

### Máquina de estados

El programa se organiza como una máquina de estados finitos con tres estados:

| Estado | Constante | Comportamiento |
|---|---|---|
| Inicio | `STATE_START` | Muestra el título y espera un toque para comenzar |
| Juego | `STATE_PLAYING` | Ejecuta la física, el desplazamiento de muros y la detección de colisiones |
| Fin de juego | `STATE_GAMEOVER` | Muestra la puntuación, reinicia las variables y espera un toque para reiniciar |

```
STATE_START ──(toque)──▶ STATE_PLAYING ──(colisión)──▶ STATE_GAMEOVER
                              ▲                              │
                              └───────────(toque)────────────┘
```

La función `loop()` actúa como despachador: en cada iteración evalúa la variable `current_state` e invoca la función correspondiente. Esta estructura es directamente comparable con las máquinas de estado implementadas en FPGA durante el Módulo I, con la diferencia de que aquí la transición se resuelve por software en lugar de lógica secuencial sincronizada.

```cpp
void loop() {
  if (current_state == STATE_PLAYING) {
    updateGame();
  } else if (current_state == STATE_GAMEOVER) {
    showGameOver();
  } else {
    showStartScreen();
  }
  delay(FRAME_DELAY);
}
```

La constante `FRAME_DELAY` (50 ms) fija la cadencia del bucle en aproximadamente 20 cuadros por segundo, valor suficiente para una animación fluida en este tipo de pantalla.

### Modelo de renderizado: el *framebuffer*

La biblioteca `Adafruit_SSD1306` mantiene en la RAM del microcontrolador una copia completa de la pantalla (1024 bytes para 128 × 64 píxeles monocromáticos). Todas las operaciones de dibujo (`fillRect`, `drawBitmap`, `print`) modifican únicamente esta copia en memoria; la pantalla física no cambia hasta que se invoca `oled.display()`, momento en el que el *framebuffer* completo se transfiere por I²C al controlador SSD1306.

Este modelo explica la secuencia repetida en cada cuadro del juego:

1. `oled.clearDisplay()` — se borra el *framebuffer* en RAM.
2. Operaciones de dibujo — se compone la escena completa en RAM.
3. `oled.display()` — se transfiere el resultado a la pantalla en una sola operación.

La transferencia de 1024 bytes a 400 kHz justifica la configuración del bus en modo rápido (`Wire1.setClock(400000)`); a la velocidad estándar de 100 kHz, el tiempo de transferencia por cuadro se cuadruplicaría y limitaría la tasa de refresco alcanzable.

## Código completo

El siguiente programa está listo para compilarse y cargarse desde el IDE de Arduino con el núcleo *arduino-pico*, una vez instaladas las bibliotecas indicadas y realizadas las conexiones de la tabla anterior.

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dimensiones del sprite del personaje
#define BIRD_SIZE 16

// Asignación de GPIO
#define I2C_SDA 2
#define I2C_SCL 3
#define BUTTON_PIN 7

// Configuración de la pantalla OLED
#define OLED_WIDTH 128  // Ancho en píxeles
#define OLED_HEIGHT 64  // Alto en píxeles
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire1, -1);

// Mapas de bits para la animación del personaje
static const unsigned char PROGMEM bird_frame1[] = {
  B00000000, B00000000,
  B00000000, B00000000,
  B00001111, B10000000,
  B00011111, B11100000,
  B00111100, B11110000,
  B01111111, B11111000,
  B11111111, B11100100,
  B11011111, B01111000,
  B11011111, B01111000,
  B11011111, B01111000,
  B11001110, B01111000,
  B01110001, B11110000,
  B01111111, B11100000,
  B00111111, B11000000,
  B00000111, B00000000,
  B00000000, B00000000
};

static const unsigned char PROGMEM bird_frame2[] = {
  B00000000, B00000000,
  B00000000, B00000000,
  B00001111, B10000000,
  B00011111, B11100000,
  B00111100, B11110000,
  B01111111, B11111000,
  B11110001, B11100100,
  B11101110, B11111000,
  B11011111, B01111000,
  B11011111, B01111000,
  B11111111, B11111000,
  B01111111, B11110000,
  B01111111, B11100000,
  B00111111, B11000000,
  B00000111, B00000000,
  B00000000, B00000000
};

// Parámetros del juego
#define FRAME_DELAY 50

// Estados del programa
#define STATE_START 0
#define STATE_PLAYING 1
#define STATE_GAMEOVER 2

int current_state = STATE_START;
int birdX = OLED_WIDTH / 4;
int birdY, gravity;
int score, bestScore;

int wallX[2];
int gapY[2];
#define GAP_SIZE 30
#define WALL_THICKNESS 10

void setup() {
  Serial.begin(115200);

  // GP2 y GP3 corresponden al controlador I2C1 del RP2040
  Wire1.setSDA(I2C_SDA);
  Wire1.setSCL(I2C_SCL);
  Wire1.begin();
  Wire1.setClock(400000);  // Modo rápido: 400 kHz

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED Initialization Failed"));
    for (;;);
  }

  oled.clearDisplay();
  oled.setTextColor(WHITE);

  // Sensor táctil activo en alto: pull-down interno
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  randomSeed(analogRead(10));
}

void loop() {
  if (current_state == STATE_PLAYING) {
    updateGame();
  } else if (current_state == STATE_GAMEOVER) {
    showGameOver();
  } else {
    showStartScreen();
  }
  delay(FRAME_DELAY);
}

void updateGame() {
  oled.clearDisplay();

  // Mecánica de aleteo: el toque produce un impulso ascendente
  if (digitalRead(BUTTON_PIN) == HIGH) gravity = -4;

  gravity++;
  birdY += gravity;

  birdY = constrain(birdY, 0, OLED_HEIGHT - BIRD_SIZE);
  if (birdY == OLED_HEIGHT - BIRD_SIZE) gravity = -2;

  // Animación del personaje
  const unsigned char *sprite = (gravity < 0 && random(2)) ? bird_frame1 : bird_frame2;
  oled.drawBitmap(birdX, birdY, sprite, BIRD_SIZE, BIRD_SIZE, WHITE);

  // Lógica de los muros
  for (int i = 0; i < 2; i++) {
    oled.fillRect(wallX[i], 0, WALL_THICKNESS, gapY[i], WHITE);
    oled.fillRect(wallX[i], gapY[i] + GAP_SIZE, WALL_THICKNESS, OLED_HEIGHT - gapY[i] + GAP_SIZE, WHITE);

    wallX[i] -= 4;

    if (wallX[i] < -WALL_THICKNESS) {
      wallX[i] = OLED_WIDTH;
      gapY[i] = random(10, OLED_HEIGHT - GAP_SIZE - 10);
    }

    if (wallX[i] == birdX) {
      score++;
      bestScore = max(bestScore, score);
    }

    // Detección de colisiones (cajas delimitadoras AABB)
    if ((birdX + BIRD_SIZE > wallX[i] && birdX < wallX[i] + WALL_THICKNESS)
        && (birdY < gapY[i] || birdY + BIRD_SIZE > gapY[i] + GAP_SIZE)) {
      current_state = STATE_GAMEOVER;
    }
  }

  oled.setCursor(OLED_WIDTH / 2 - 6, 0);
  oled.print(score);
  oled.display();
}

void showGameOver() {
  wipeScreen();
  oled.setCursor(OLED_WIDTH / 2 - 30, OLED_HEIGHT / 2 - 20);
  oled.print(F("GAME OVER"));

  oled.setCursor(OLED_WIDTH / 2 - 3, OLED_HEIGHT / 2 - 10);
  oled.print(score);

  oled.setCursor(OLED_WIDTH / 2 - 30, OLED_HEIGHT - 26);
  oled.print(F("BEST SCORE"));

  oled.setCursor(OLED_WIDTH / 2 - 3, OLED_HEIGHT - 18);
  oled.print(bestScore);

  oled.display();

  // Espera a que se retire el contacto que provocó la colisión
  while (digitalRead(BUTTON_PIN) == HIGH);

  birdY = OLED_HEIGHT / 2;
  gravity = -4;
  score = 0;
  wallX[0] = OLED_WIDTH;
  gapY[0] = OLED_HEIGHT / 2 - GAP_SIZE / 2;
  wallX[1] = OLED_WIDTH + OLED_WIDTH / 2;
  gapY[1] = random(10, OLED_HEIGHT - GAP_SIZE - 10);

  // Espera un nuevo toque para reiniciar
  while (digitalRead(BUTTON_PIN) == LOW);

  wipeScreen();
  current_state = STATE_PLAYING;
}

void showStartScreen() {
  oled.clearDisplay();
  oled.setCursor(OLED_WIDTH / 2 - 30, OLED_HEIGHT / 2 - 8);
  oled.print(F("ESP BIRD"));
  oled.setCursor(OLED_WIDTH / 2 - 42, OLED_HEIGHT / 2 + 8);
  oled.print(F("Press to Start"));
  oled.display();

  if (digitalRead(BUTTON_PIN) == HIGH) {
    wipeScreen();
    birdY = OLED_HEIGHT / 2;
    gravity = -4;
    score = 0;
    wallX[0] = OLED_WIDTH;
    gapY[0] = OLED_HEIGHT / 2 - GAP_SIZE / 2;
    wallX[1] = OLED_WIDTH + OLED_WIDTH / 2;
    gapY[1] = random(10, OLED_HEIGHT - GAP_SIZE - 10);
    current_state = STATE_PLAYING;
  }
}

void wipeScreen() {
  oled.clearDisplay();
  oled.display();
}
```

## Descripción del código

### Configuración de la comunicación I²C

```cpp
Wire1.setSDA(I2C_SDA);   // GP2
Wire1.setSCL(I2C_SCL);   // GP3
Wire1.begin();
Wire1.setClock(400000);  // Modo rápido: 400 kHz
```

El RP2040 dispone de dos controladores I²C independientes (I2C0 e I2C1). Cada GPIO admite únicamente uno de los dos controladores, según la tabla de multiplexado del microcontrolador: los GPIO 2 y 3 corresponden a **I2C1**, por lo que el objeto correcto es `Wire1` y no `Wire`. El núcleo *arduino-pico* permite además reasignar los pines mediante `setSDA()` y `setSCL()` **antes** de invocar `begin()`, flexibilidad de la que carecen plataformas con pines I²C fijos.

El objeto de comunicación se vincula a la pantalla en la declaración global:

```cpp
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire1, -1);
```

El cuarto argumento (`-1`) indica que no se utiliza pin de *reset* dedicado; el módulo OLED empleado gestiona su reinicio internamente.

### Inicialización de la pantalla

```cpp
if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println(F("OLED Initialization Failed"));
  for (;;);
}
```

`SSD1306_SWITCHCAPVCC` habilita el elevador de tensión interno (*charge pump*) del controlador, que genera los ~7.5 V requeridos por el panel OLED a partir de la alimentación de 3.3 V. La dirección `0x3C` es la dirección I²C estándar de estos módulos. Si la inicialización falla —cableado incorrecto o dirección distinta—, el programa se detiene en un bucle infinito tras reportar el error por el puerto serie, evitando continuar en un estado inconsistente.

### Configuración de la entrada digital

```cpp
pinMode(BUTTON_PIN, INPUT_PULLDOWN);
```

El sensor táctil es un dispositivo **activo en alto**: su salida permanece en nivel bajo en reposo y conmuta a nivel alto al detectar contacto. La resistencia interna de *pull-down* garantiza un nivel bajo definido en ausencia de señal, evitando lecturas flotantes.

En consecuencia, todas las lecturas del programa comparan contra `HIGH` para detectar el toque:

```cpp
if (digitalRead(BUTTON_PIN) == HIGH) gravity = -4;
```

::: warning Polaridad de la entrada: un error frecuente de portabilidad
El código original para ESP32 empleaba un botón mecánico con *pull-up*, es decir, **activo en bajo** (reposo = `HIGH`, presionado = `LOW`). Al sustituir el botón por el sensor táctil sin invertir las comparaciones, el programa interpreta el estado de reposo como pulsación permanente: el personaje asciende sin control y solo desciende mientras se mantiene el contacto. La corrección consiste en invertir las cuatro comparaciones de `digitalRead()` del programa y configurar el pin con `INPUT_PULLDOWN`. Este caso ilustra la importancia de verificar la polaridad lógica de cada dispositivo de entrada antes de reutilizar código.
:::

### Física del juego

El movimiento vertical del personaje se modela mediante una integración numérica simplificada, ejecutada una vez por cuadro:

```cpp
if (digitalRead(BUTTON_PIN) == HIGH) gravity = -4;  // Impulso ascendente

gravity++;          // Aceleración constante hacia abajo
birdY += gravity;   // Integración de la velocidad en la posición
```

La variable `gravity` funciona como velocidad vertical: el toque la fija en un valor negativo (impulso hacia arriba, dado que el origen de coordenadas de la pantalla está en la esquina superior izquierda) y en cada cuadro se incrementa en una unidad, simulando la aceleración gravitatoria. La función `constrain()` limita la posición al área visible de la pantalla.

### Sprites y animación

El personaje se define mediante dos mapas de bits de 16 × 16 píxeles (`bird_frame1`, `bird_frame2`), declarados byte a byte con la notación binaria `B00000000` de Arduino, en la que cada bit representa un píxel. Durante el ascenso, el programa alterna aleatoriamente entre ambos cuadros para producir el efecto de aleteo:

```cpp
const unsigned char *sprite = (gravity < 0 && random(2)) ? bird_frame1 : bird_frame2;
oled.drawBitmap(birdX, birdY, sprite, BIRD_SIZE, BIRD_SIZE, WHITE);
```

::: info Sobre el calificador `PROGMEM`
En la arquitectura AVR (Arduino UNO), `PROGMEM` es indispensable para ubicar datos constantes en memoria de programa y no consumir la escasa SRAM. En el RP2040, la arquitectura ARM Cortex-M0+ presenta un espacio de direcciones unificado y el código se ejecuta directamente desde la memoria flash externa (*execute-in-place*, XIP); el núcleo *arduino-pico* define `PROGMEM` como una macro sin efecto, mantenida únicamente por compatibilidad. Los datos declarados `const` residen en flash de manera natural. Conservar el calificador no perjudica y facilita la portabilidad del código hacia otras plataformas Arduino.
:::

### Lógica de los obstáculos

El juego mantiene dos muros simultáneos, gestionados mediante los arreglos `wallX[2]` (posición horizontal) y `gapY[2]` (posición vertical de la abertura). En cada cuadro, para cada muro:

1. Se dibujan los dos rectángulos que delimitan la abertura de `GAP_SIZE` (30) píxeles.
2. La posición horizontal se decrementa en 4 píxeles, produciendo el desplazamiento hacia la izquierda.
3. Cuando el muro sale por el borde izquierdo, se reposiciona en el borde derecho con una abertura en posición vertical aleatoria.
4. Si la posición del muro coincide con la del personaje, la puntuación se incrementa.

### Detección de colisiones

```cpp
if ((birdX + BIRD_SIZE > wallX[i] && birdX < wallX[i] + WALL_THICKNESS)
    && (birdY < gapY[i] || birdY + BIRD_SIZE > gapY[i] + GAP_SIZE)) {
  current_state = STATE_GAMEOVER;
}
```

La condición implementa la técnica de **cajas delimitadoras alineadas a los ejes** (AABB, *axis-aligned bounding boxes*): existe colisión cuando hay superposición horizontal entre el personaje y el muro **y**, simultáneamente, el personaje sobresale de la abertura por arriba o por abajo. La detección de la colisión produce la transición al estado `STATE_GAMEOVER`.

### Pantallas de inicio y de fin de juego

Ambas pantallas emplean las primitivas de texto de `Adafruit_GFX` (`setCursor`, `print`). La pantalla de fin de juego presenta además dos esperas bloqueantes secuenciales:

```cpp
while (digitalRead(BUTTON_PIN) == HIGH);  // Espera a que se retire el contacto
// ... reinicio de variables ...
while (digitalRead(BUTTON_PIN) == LOW);   // Espera un nuevo toque para reiniciar
```

La primera espera evita que el contacto que provocó la colisión —o que aún se mantiene— reinicie el juego de inmediato; la segunda detiene el programa hasta recibir la orden explícita de reinicio. Esta técnica de sincronización por sondeo (*polling*) resulta aceptable en estados donde no existe otra actividad concurrente. En el estado de juego, en cambio, la lectura del sensor es no bloqueante, pues la animación debe continuar en cada cuadro.

## Adaptación de ESP32 a RP2040

La migración del programa original requirió tres modificaciones, resumidas a continuación:

| Aspecto | ESP32 (original) | RP2040 (adaptado) |
|---|---|---|
| Objeto I²C | `Wire` con pines por defecto | `Wire1` con `setSDA(2)` / `setSCL(3)`, según el multiplexado del RP2040 |
| Entrada | Botón mecánico, activo en bajo, `INPUT_PULLUP` | Sensor táctil, activo en alto, `INPUT_PULLDOWN` |
| Comparaciones de lectura | `digitalRead(...) == LOW` para detectar pulsación | `digitalRead(...) == HIGH` para detectar toque |

Conviene destacar que la adaptación no exigió cambio alguno en la lógica del juego, en las bibliotecas gráficas ni en el protocolo I²C: la abstracción que proporciona el entorno Arduino confina las diferencias entre plataformas a la configuración de periféricos y a las características eléctricas de los dispositivos conectados.

## Ejercicios propuestos

1. **Rebote de la entrada.** Sustituir la lectura por sondeo del sensor por una detección de flanco ascendente por interrupción (`attachInterrupt` con `RISING`), de modo que cada toque produzca exactamente un impulso, independientemente de su duración.
2. **Dificultad progresiva.** Incrementar la velocidad de desplazamiento de los muros o reducir `GAP_SIZE` conforme aumenta la puntuación.
3. **Persistencia de la puntuación máxima.** La variable `bestScore` se pierde al desconectar la alimentación. Investigar la biblioteca `EEPROM` del núcleo *arduino-pico* (emulada sobre un sector de la memoria flash) para conservar el valor entre sesiones.
4. **Corrección geométrica.** El segundo rectángulo de cada muro se dibuja con altura `OLED_HEIGHT - gapY[i] + GAP_SIZE`, expresión que excede el borde inferior de la pantalla. Determinar la expresión correcta y verificar que el resultado visual no cambia, razonando por qué el recorte automático de la biblioteca oculta el error.
5. **Sonido.** Agregar un zumbador pasivo en un GPIO libre y generar tonos con `tone()` al puntuar y al colisionar.

## Referencias

- Raspberry Pi Ltd., *RP2040 Datasheet*, sección 2.19 (I²C) y tabla de funciones de GPIO (sección 2.19.2).
- Earle Philhower, *Arduino-Pico Documentation* — Wire (I²C) support.
- Adafruit Industries, *Adafruit GFX Graphics Library* y *Monochrome OLED Breakouts* (guías de uso).
