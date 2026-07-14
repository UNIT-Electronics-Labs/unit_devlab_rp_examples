# Juego Gato (Tic-Tac-Toe) con Display OLED: Máquina de Estados y Adaptación de Hardware en Arduino

## Introducción

Los ejemplos anteriores de esta sección abordan un periférico o un mecanismo
del RP2040 de forma aislada. El presente ejemplo tiene un propósito distinto:
mostrar cómo un programa de mayor complejidad —organizado como una máquina de
estados con entrada de usuario, lógica de decisión y salida gráfica— se
adapta de una plataforma AVR de 8 bits (Arduino Uno/Nano) al RP2040, sin
alterar la lógica del juego en sí. El código original fue publicado por
Giovanni Verrua (abr/2022) para Arduino Uno/Nano/Mini Pro; la adaptación
presentada aquí conserva íntegramente la lógica de juego y modifica
únicamente los tres aspectos que dependen del hardware de destino: la lógica
eléctrica de los botones, la configuración de pines del bus I²C, y la
asignación específica de pines conforme al montaje del UNIT DevLab
MultiHub Shield.

Este ejemplo resulta útil como cierre de la sección Arduino porque combina,
en un solo programa, tres elementos ya vistos por separado: entrada digital
mediante botones, comunicación I²C con un display OLED, y una estructura de
programa basada en estados en lugar de un `loop()` lineal.

## 1. Migración de una plataforma AVR de 8 bits al RP2040

| Aspecto | Arduino Uno/Nano (AVR, 8 bits) | RP2040 (Cortex-M0+, 32 bits) |
|---|---|---|
| Resistencias de polarización de botones | Requiere resistencias externas (el código original indica pull-down de 10-100 kΩ) | Puede resolverse con resistencias externas en la placa de expansión, sin necesidad de `INPUT_PULLUP`/`INPUT_PULLDOWN` por software |
| Bus I²C | Pines fijos A4 (SDA) / A5 (SCL), sin remapeo posible | Múltiples GPIO asignables a I2C0/I2C1 mediante `Wire.setSDA()`/`Wire.setSCL()` |
| Frecuencia de CPU | 16 MHz | 133 MHz (por defecto) |
| Comportamiento de la lógica de botones | Determinado por el circuito externo (pull-down: reposo en LOW, presionado en HIGH) | Idéntico si se conserva el mismo circuito externo; no requiere cambios de lógica por el cambio de arquitectura |

El punto central de esta migración es que **ninguna de las diferencias
arquitectónicas entre AVR y Cortex-M0+ obliga, por sí misma, a modificar la
lógica del juego**. Los cambios realizados respecto al código original no
provienen de la diferencia de arquitectura de CPU, sino de una decisión de
diseño de hardware específica de este montaje: en el UNIT DevLab MultiHub
Shield, las resistencias de polarización de los botones ya están resueltas
en la placa (a diferencia del montaje original en protoboard del ejemplo de
Verrua, que requería resistencias externas discretas). Por ello, la
configuración de pines en `setup()` se limita a `pinMode(BUTTON, INPUT)`, sin
`INPUT_PULLUP` ni `INPUT_PULLDOWN`: la polarización ya está resuelta
íntegramente en hardware, y añadir una resistencia de polarización interna
por software sobre un pin que ya tiene su propia resistencia externa podría
producir una condición de conflicto eléctrico entre ambas si sus sentidos no
coinciden.

## 2. Cambios realizados respecto al código original

Los cambios respecto a la versión de Giovanni Verrua se limitan estrictamente
a tres puntos, todos ellos dependientes del hardware de destino y no de la
lógica del juego:

1. **Lógica eléctrica de los botones**: en el UNIT DevLab MultiHub Shield,
   los botones ya cuentan con su propia red de polarización en la placa. Se
   ajustó la lectura (`digitalRead(BOTON) == LOW`) para corresponder al
   sentido eléctrico real de este montaje, en lugar de las resistencias pull-down
   externas propuestas en el esquema original.
2. **Configuración de pines I²C**: se emplea `Wire.setSDA()` / `Wire.setSCL()`
   antes de `Wire.begin()`, ya que el RP2040 permite asignar el bus I2C0 a
   distintos pares de GPIO, a diferencia de los pines fijos A4/A5 del
   Arduino Uno/Nano original.
3. **Asignación específica de pines**: los números de pin de los botones
   (`BUTTON_MOVE`, `BUTTON_OK`) y del bus I²C (`SDA_PIN`, `SCL_PIN`) se
   ajustaron a la disposición física real del UNIT DevLab MultiHub Shield.

La lógica de juego —el algoritmo de decisión de la CPU en `checkboard()`, la
máquina de estados de `loop()`, el dibujo del tablero y el manejo del
parpadeo del cursor— se conserva sin modificaciones respecto al original.

## 3. Estructura del programa: máquina de estados frente a `loop()` lineal

A diferencia de los ejemplos anteriores de esta sección, donde `loop()`
ejecuta la misma secuencia de instrucciones en cada iteración, este programa
organiza su ejecución mediante una variable de estado (`gameStatus`) que
determina qué bloque de código se ejecuta en cada paso:

| Valor de `gameStatus` | Bloque ejecutado |
|---|---|
| `0` | Reinicio del tablero (`resetGame()`) y dibujo del tablero vacío |
| `1` | Bucle de partida: alterna turnos entre CPU y jugador humano hasta que exista un ganador o empate |

Dentro del estado `1`, una segunda variable (`winner`) controla la
permanencia en el bucle de partida: se inicializa en `0` (sin ganador) y
cambia a `1` (gana el jugador), `2` (gana la CPU) o `3` (empate) una vez que
`checkWinner()` detecta una condición de cierre. Este patrón —una variable de
estado externa al `loop()` que decide el bloque activo, en lugar de anidar
toda la lógica dentro de un único ciclo— es una estructura de diseño de
software aplicable independientemente de la plataforma, y resulta análoga a
una máquina de estados finita implementada en software.

## 4. El algoritmo de la CPU: heurística de bloqueo/victoria

La función `checkboard(x)` recorre las ocho combinaciones ganadoras posibles
del tablero (tres filas, tres columnas, dos diagonales) buscando una
condición donde dos de las tres celdas ya pertenezcan al jugador `x` y la
tercera esté vacía. La función `playcpu()` invoca `checkboard()` dos veces
con distinto argumento:

1. Primero con `x = 2` (CPU): si existe una celda que complete una línea de
   la CPU, se ocupa esa celda y la partida se gana de inmediato.
2. Si no existe movimiento ganador para la CPU, se invoca nuevamente con
   `x = 1` (jugador humano): si el jugador tiene una línea a punto de
   completarse, la CPU ocupa esa celda para bloquear la victoria.
3. Si ninguna de las dos condiciones anteriores se cumple, la CPU selecciona
   una celda vacía de forma aleatoria mediante `random()`.

Esta heurística no constituye una búsqueda exhaustiva del árbol de jugadas
(como sí lo sería un algoritmo Minimax completo); se trata de una regla fija
de dos pasos (ganar si es posible, bloquear si es necesario, aleatorio en
cualquier otro caso). El comentario del autor original señala correctamente
que esta heurística es difícil de vencer en la práctica, aunque no es
matemáticamente óptima: existen posiciones donde el jugador humano genera
dos amenazas de victoria simultáneas ("doble amenaza") que esta heurística no
anticipa, dado que solo evalúa bloqueos de una amenaza a la vez.

## 5. Manejo de la entrada de usuario sin bloqueo indefinido

La función `playhuman()` implementa la selección de celda mediante dos
botones: uno (`BUTTON_MOVE`) desplaza un cursor parpadeante ("?") a la
siguiente celda vacía, y otro (`BUTTON_OK`) confirma la selección. El cursor
parpadeante se implementa comparando `millis()` contra una marca de tiempo
almacenada, alternando su visibilidad cada 200 ms sin recurrir a `delay()`
bloqueante para el parpadeo en sí (aunque el bucle de sondeo de botones sí
incluye un `delay(100)` menor, necesario según el comentario del autor
original para el refresco correcto del display).

Un aspecto importante del diseño, señalado explícitamente en los comentarios
del código original, es que el bucle interno `while (board[humanMove] != 0)`
que busca la siguiente celda vacía **no** verifica un límite de intentos: si
el tablero estuviera completamente lleno, ese bucle no terminaría. Esta
condición está garantizada por diseño desde `loop()`: `checkWinner()` se
invoca después de cada movimiento y determina un ganador o empate en cuanto
el tablero se llena, de modo que `playhuman()` nunca se invoca con el
tablero completo. Se trata de una invariante de programa —una condición que
el flujo de control garantiza indirectamente— y no de una verificación
defensiva dentro de la función misma.

## 6. Ejemplo aplicado: código completo

### Hardware y Conexiones

| Señal | Dispositivo | GPIO RP2040 |
|---|---|---|
| SDA | SSD1306 | GP0 |
| SCL | SSD1306 | GP1 |
| VCC | SSD1306 | 3V3 |
| GND | SSD1306 | GND |
| BUTTON_MOVE | Botón "mover cursor" | GP8 |
| BUTTON_OK | Botón "confirmar" | GP9 |

Los botones cuentan con su propia red de polarización en el UNIT DevLab
MultiHub Shield; no se requiere resistencia externa adicional ni
`INPUT_PULLUP`/`INPUT_PULLDOWN` por software.

### Bibliotecas requeridas (Instalación)

Instalables desde el Gestor de Bibliotecas del Arduino IDE:

- **Adafruit SSD1306** (requiere como dependencia **Adafruit GFX Library**)

### Código completo

```cpp
/**************************************************************************
 *
 *  TicTacToe - Oled
 *  Adaptado para RP2040 (UNIT DevLab MultiHub Shield)
 *
 *  Codigo original para Arduino Nano/Uno/Mini Pro:
 *  Giovanni Verrua, abr/2022
 *
 *  Cambios realizados en esta adaptacion:
 *   - Logica electrica de los botones: en este montaje, la polarizacion ya
 *     esta resuelta en el UNIT DevLab MultiHub Shield (sin resistencias
 *     externas ni INPUT_PULLUP/INPUT_PULLDOWN por software).
 *   - Configuracion de pines I2C mediante Wire.setSDA()/setSCL(), ya que el
 *     RP2040 permite asignar el bus a distintos GPIO (a diferencia de los
 *     pines fijos A4/A5 del Arduino Uno/Nano original).
 *   - Asignacion especifica de pines conforme al montaje real.
 *
 * Hardware connections:
 *  DISPLAY (I2C)     - RP2040
 *         GND        -    GND
 *         VDD        -    3V3
 *         SCL        -    GP1
 *         SDA        -    GP0
 *
 *   GP8 <-- button MOVE
 *   GP9 <-- button OK
 *   (polarizacion resuelta en el UNIT DevLab MultiHub Shield)
 *
 * Install the Adafruit SSD1306 libraries
 * by Arduino IDE, menu Tools -> Manage Libraries
 *
**************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_MOVE 8
#define BUTTON_OK 9
#define SDA_PIN 0
#define SCL_PIN 1
#define OLED_RESET 4 // Reset pin # (o -1 si comparte el reset de la placa)

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

int gameStatus = 0;
int whosplaying = 0; //0 = Arduino, 1 = Human

int winner = -1;  //-1 = Playing, 0 = Draw, 1 = Human, 2 = CPU

int board[]={0,0,0,
             0,0,0,
             0,0,0}; //0 = blank, 1 = human (circle), 2 = computer (cross)

//--------------------------------------------------------------------------------------------------------
void playhuman() {

    int humanMove = 0;

    bool stayInLoop = true;
    bool showDot = false;
    long timerPos = millis()-1000;

    while (stayInLoop) {  //stay in loop until the player makes his/her choice hitting the OK button.

        //If the current "?" position isn't avaliable (the cell value is 1 or 2), this loop will
        //move the "?" to the next free cell.
        //NOTE: there must be at least one empty cell (or it will never exit from this loop [deadlock]).
        //This is granted, because if all the cells are used, there's a winner or it's a draft.
        //(the calling function [loop function] check it before to continue).

        while (board[humanMove] != 0) {  //looking for an empty cell.
            humanMove ++;
            if (humanMove >8) humanMove = 0;
        }

        //--------------------------------------------------\\-
        //this makes the flashing "?" possible. Every 200 milliseconds the IF condition becomes true and it will toogle the
        //showDot variable between True and False (and reset the timerPos value at the current millis() value.
        if (timerPos + 200 < millis()) {
            timerPos = millis();
            showDot = !showDot;
            playhuman_showpos( humanMove, showDot);  //calling the function that will draw (or delete) the "?"
        }
        //--------------------------------------------------/-

        if (digitalRead(BUTTON_MOVE)==LOW) {   //the player hit the MOVE button.
            playhuman_showpos( humanMove, false);  //delete the marker
            humanMove ++; //move the "?" to the next cell.

            while (digitalRead(BUTTON_MOVE)==LOW); //debounce

            bool showDot = false;  //this two lines make sure the "?" is displayed
            long timerPos =-1000;  //at the first round.
        }

        if (digitalRead(BUTTON_OK)==LOW) stayInLoop = false;  //the player hit the OK button and made his/her choice.

        delay(100); //required for a correct display.
    }

    board[humanMove] = 1;   //let's assign the chosen cell to the player.

}

//------------------------------------------------------------------

void playhuman_showpos(int humanMove, bool showDot) {   //this function draw a flashing "?"  (white = draw, black = delete)

    display.setTextSize(2);
    if (humanMove == 0) display.setCursor( 5, 5);
    else if (humanMove == 1) display.setCursor(25, 5);
    else if (humanMove == 2) display.setCursor(45, 5);
    else if (humanMove == 3) display.setCursor( 5,25);
    else if (humanMove == 4) display.setCursor(25,25);
    else if (humanMove == 5) display.setCursor(45,25);
    else if (humanMove == 6) display.setCursor( 5,45);
    else if (humanMove == 7) display.setCursor(25,45);
    else if (humanMove == 8) display.setCursor(45,45);

    if (showDot) display.setTextColor(WHITE); else display.setTextColor(BLACK);

    display.print("?");
    display.display();
}

//--------------------------------------------------------------------------------------------------------
void playcpu() {

    //NOTE: The player has almost no chance to win, since the cpu will check every possible move.
    //      Actually the only way to beat the cpu is to have two winning move at the same time.

    //The CPU has no real strategy, actually; it just prevents the player to win and put an "X" if it has
    //a possible winning move. If no winning move are possible, it just put an "X" into a random place.
    //It could seems a stupid AI, however you will see the CPU will play rather well and it will be
    //hard to beat it.

    int cpumove = checkboard(2);  //2 = cpu  let's check if there's a cpu's winner move

    if (cpumove >=0) {
        board[cpumove] = 2;    //cpu's winner move
    }
    else {
        cpumove = checkboard(1);   //1=player check if the player has a chance to win (2 circles and an empty cell in a row)
        if (cpumove >=0) {
            board[cpumove] = 2;   //this move will break the player's winner move
        }

        //there's no possible winner move neither for the cpu, nor for the human;: the CPU will put an "X" in a random cell
        while (cpumove < 0) {   //looking for a random, empty cell.
            int randomMove = random(10);
            if (randomMove >=0 && randomMove <=8 && board[randomMove] == 0) {
                cpumove = randomMove;
            }
        }
        board[cpumove] = 2;  //let's assign the empty cell to the CPU
    }
}


//--------------------------------------------------------------------------------------------------------
int checkboard(int x){   //x = 1 -> player, x = 2 -> cpu

    //this function checks if the next move can be the winning move and return the cell that will
    //win the game. It's used by the CPU to decide if it can win or if the player is going to win
    //(and placing an "X" to prevent this chance).
    //if no move wins the game, it returns -1
    //the board[] index is 0 1 2
    //                     3 4 5
    //                     6 7 8

        if (board[0]==0 && board[1]==x && board[2]==x)  return  0;  //  0 1 1
                                                                     //  . . .
                                                                     //  . . .

    else if (board[0]==x && board[1]==0 && board[2]==x)  return  1;  //  1 0 1
                                                                      //  . . .
                                                                      //  . . .

    else if (board[0]==x && board[1]==x && board[2]==0)  return  2;  //  1 1 0
                                                                      //  . . .
                                                                      //  . . .
    //-------------------------------------------------
    else if (board[3]==0 && board[4]==x && board[5]==x)  return  3;  //  . . .
                                                                      //  0 1 1
                                                                      //  . . .

    else if (board[3]==x && board[4]==0 && board[5]==x)  return  4;  //  . . .
                                                                      //  1 0 1
                                                                      //  . . .

    else if (board[3]==x && board[4]==x && board[5]==0)  return  5;  //  . . .
                                                                      //  1 1 0
                                                                      //  . . .
    //-------------------------------------------------
    else if (board[6]==0 && board[7]==x && board[8]==x)  return  6;  //  . . .
                                                                      //  . . .
                                                                      //  0 1 1

    else if (board[6]==x && board[7]==0 && board[8]==x)  return  7;  //  . . .
                                                                      //  . . .
                                                                      //  1 0 1

    else if (board[6]==x && board[7]==x && board[8]==0)  return  8;  //  . . .
                                                                      //  . . .
                                                                      //  1 1 0

    //-------------------------------------------------
    else if (board[0]==0 && board[3]==x && board[6]==x)  return  0;  //  0 . .
                                                                      //  1 . .
                                                                      //  1 . .

    else if (board[0]==x && board[3]==0 && board[6]==x)  return  3;  //  1 . .
                                                                      //  0 . .
                                                                      //  1 . .

    else if (board[0]==x && board[3]==x && board[6]==0)  return  6;  //  1 . .
                                                                      //  1 . .
                                                                      //  0 . .

    //-------------------------------------------------
    else if (board[1]==0 && board[4]==x && board[7]==x)  return  1;  //  . 0 .
                                                                      //  . 1 .
                                                                      //  . 1 .

    else if (board[1]==x && board[4]==0 && board[7]==x)  return  4;  //  . 1 .
                                                                      //  . 0 .
                                                                      //  . 1 .

    else if (board[1]==x && board[4]==x && board[7]==0)  return  7;  //  . 1 .
                                                                      //  . 1 .
                                                                      //  . 0 .

    //-------------------------------------------------
    else if (board[2]==0 && board[5]==x && board[8]==x)  return  2;  //  . . 0
                                                                      //  . . 1
                                                                      //  . . 1

    else if (board[2]==x && board[5]==0 && board[8]==x)  return  5;  //  . . 1
                                                                      //  . . 0
                                                                      //  . . 1

    else if (board[2]==x && board[5]==x && board[8]==0)  return  8;  //  . . 1
                                                                      //  . . 1
                                                                      //  . . 0

    //-------------------------------------------------
    else if (board[0]==0 && board[4]==x && board[8]==x)  return  0;  //  0 . .
                                                                      //  . 1 .
                                                                      //  . . 1

    else if (board[0]==x && board[4]==0 && board[8]==x)  return  4;  //  1 . .
                                                                      //  . 0 .
                                                                      //  . . 1

    else if (board[0]==x && board[4]==x && board[8]==0)  return  8;  //  1 . .
                                                                      //  . 1 .
                                                                      //  . . 0

    //-------------------------------------------------
    else if (board[2]==0 && board[4]==x && board[6]==x)  return  2;  //  . . 0
                                                                      //  . 1 .
                                                                      //  1 . .

    else if (board[2]==x && board[4]==0 && board[6]==x)  return  4;  //  . . 1
                                                                      //  . 0 .
                                                                      //  1 . .

    else if (board[2]==x && board[4]==x && board[6]==0)  return  6;  //  . . 1
                                                                      //  . 1 .
                                                                      //  0 . .

    else                                                 return -1;
}

//--------------------------------------------------------------------------------------------
void checkWinner() {    //check the board to see if there is a winner

    winner = 3;  //3=draft, 1= winner->player, 2=winner->cpu

    // circles win?
    if (board[0]==1 && board[1]==1 && board[2]==1)     winner=1;
    else if (board[3]==1 && board[4]==1 && board[5]==1)     winner=1;
    else if (board[6]==1 && board[7]==1 && board[8]==1)     winner=1;
    else if (board[0]==1 && board[3]==1 && board[6]==1)     winner=1;
    else if (board[1]==1 && board[4]==1 && board[7]==1)     winner=1;
    else if (board[2]==1 && board[5]==1 && board[8]==1)     winner=1;
    else if (board[0]==1 && board[4]==1 && board[8]==1)     winner=1;
    else if (board[2]==1 && board[4]==1 && board[6]==1)     winner=1;

    // crosses win?
    else if (board[0]==2 && board[1]==2 && board[2]==2)     winner=2;
    else if (board[3]==2 && board[4]==2 && board[5]==2)     winner=2;
    else if (board[6]==2 && board[7]==2 && board[8]==2)     winner=2;
    else if (board[0]==2 && board[3]==2 && board[6]==2)     winner=2;
    else if (board[1]==2 && board[4]==2 && board[7]==2)     winner=2;
    else if (board[2]==2 && board[5]==2 && board[8]==2)     winner=2;
    else if (board[0]==2 && board[4]==2 && board[8]==2)     winner=2;
    else if (board[2]==2 && board[4]==2 && board[6]==2)     winner=2;

    if (winner == 3) {
        for(int i=0;i<9;i++) if (board[i]==0)   winner=0;  //there are some empty cells yet.
    }

}

//--------------------------------------------------------------------------------------------------------------

void resetGame() {

    for(int i=0;i<9;i++) board[i]=0;   //Resetting the board. 0 = empty cell, 1 = player circle, 2 = CPU cross

    winner = 0;
    gameStatus = 0;

}

//--------------------------------------------------------------------------------------------------------------

void boardDrawing() {

    display.clearDisplay();
    display.setTextColor(WHITE);

    display.drawFastHLine(0, 21, 64, WHITE); //horizontal lines
    display.drawFastHLine(0, 42, 64, WHITE);

    display.drawFastVLine(21, 0, 64, WHITE); //vertical lines
    display.drawFastVLine(42, 0, 64, WHITE);

    //drawing the content of the nine cells: " ", "o", "x"
    display.setTextSize(2);
    display.setCursor( 5, 5); display.print(charBoard(0));  display.setCursor(25, 5); display.print(charBoard(1)); display.setCursor(45, 5); display.print(charBoard(2));
    display.setCursor( 5,25); display.print(charBoard(3));  display.setCursor(25,25); display.print(charBoard(4)); display.setCursor(45,25); display.print(charBoard(5));
    display.setCursor( 5,45); display.print(charBoard(6));  display.setCursor(25,45); display.print(charBoard(7)); display.setCursor(45,45); display.print(charBoard(8));
    display.display();

    delay(200); //DON'T REMOVE!!!! needed for correct refresh and further flashing "?" when it's the player turn!!!
}

//--------------------------------------------------------------------------------------------------------------
String charBoard(int x) {
    if (board[x] == 0) return " ";
    if (board[x] == 1) return "o";
    if (board[x] == 2) return "x";

    return "?";  //error trap; but it's impossible it can return an "?" because the board[] array is all initialized = 0
}

//--------------------------------------------------------------------------------------------------------------
void setup() {  //function executed once, at every boot or reset.

    randomSeed(analogRead(0));   //resetting the random function behavior.
    Wire.setSCL(SCL_PIN);
    Wire.setSDA(SDA_PIN);
    Wire.setClock(400000);
    pinMode(BUTTON_MOVE,INPUT);     //polarizacion resuelta en el UNIT DevLab MultiHub Shield
    pinMode(BUTTON_OK,INPUT);       //polarizacion resuelta en el UNIT DevLab MultiHub Shield

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    delay(500);                   //needed for display correct initializing
    display.clearDisplay();       //clearing the display
    display.setTextColor(WHITE);  //setting the display color
    display.display();            //executing the above instructions. The SSD1306 dislay will not execute any command until to use the ::display() command.

    whosplaying = 2;  //deciding who's the first player. Set = 2 to force it entering in the following while loop.
    while ( whosplaying <0 || whosplaying > 1) whosplaying = random(2);  //it will stay in the loop until whosplaying isn't = 0 or = 1. Probably there's no
                                                                          //need for a loop, since random(2) should return 0 or 1, but I'm an old programmer,
                                                                          //I've seen many strange things in my programmer life and I like to be sure ;-)
}


//--------------------------------------------------------------------------------------------------------------
void loop() {   //main loop. Endlessly executed by Arduino.

    if (gameStatus == 0){     //this is where I always put a menu in the Arduino games I wrote. We don't need a menu here, so it's just a reset step.
        resetGame();
        boardDrawing();     //drawing an empty board
        gameStatus = 1;     //starting the game (see below)
        winner = 0;         //no winner for now (winner = 1: player, winner = 2: cpu).
    }

    //---------------------------------------------

    if (gameStatus == 1){   //starting the game

        while (winner == 0) {  //game main loop: loop until no one wins the match.

            display.setTextSize(2);

            if (whosplaying == 0) {  //whosplaying = 0: cpu turn

                display.setCursor( 72,25); display.print("CPU");
                display.display();
                delay(1000);

                playcpu();    //in this function the CPU play its move.

                whosplaying =1;    //changing the turn.
            }
            else {

                display.setCursor( 72,25); display.print("You");
                display.display();

                playhuman();  //in this function the player makes his/her move.

                whosplaying =0;  //changing the turn.
            }

            boardDrawing();   //refreshing the board with all the moves already done.
            delay(500);

            checkWinner();  //this will check if there's a winner and assign the winner variable

            if (winner > 0) {

                if (winner == 3) {
                    display.setTextSize(2); display.setCursor( 68, 25);
                    display.print("Draft");
                }
                else {
                    //showing who's the winner
                    display.setTextSize(2); display.setCursor( 72, 25);
                    if (winner == 1) { Serial.println(F("You")); display.print("You"); display.setCursor( 72, 45); display.print("win"); }
                    else             { Serial.println(F("CPU")); display.print("CPU"); display.setCursor( 72, 45); display.print("wins");}

                }
                display.display();
                delay(1000);

                //debounce loop. It will not proceed until both buttons are unpressed.
                while (digitalRead(BUTTON_MOVE)==HIGH && digitalRead(BUTTON_OK)==HIGH);

            }

        }

        //swap the first move for the next match between CPU and human (one per match)
        if (whosplaying == 0) whosplaying =1; else whosplaying =0;

        gameStatus = 0;  //entering the reset step
        delay(1000);     //just wait a second

    }

}
```

## 7. Verificación

Al finalizar la carga, el display muestra el tablero vacío (dos líneas
horizontales y dos verticales dividiendo la pantalla en nueve celdas) y, en
la columna derecha, la indicación de turno ("CPU" o "You"). Durante el turno
del jugador humano, un símbolo "?" parpadea sobre la celda actualmente
seleccionada; al presionar `BUTTON_MOVE`, el cursor avanza a la siguiente
celda vacía; al presionar `BUTTON_OK`, la celda queda marcada con "o". Al
completarse una línea o llenarse el tablero, el display muestra "You win",
"CPU wins" o "Draft" según corresponda, y el programa espera a que ambos
botones se suelten antes de reiniciar automáticamente una nueva partida.

<div align="center"><img src="/images/arduino_tictactoe_oled.png" alt="Verificacion juego Tic-Tac-Toe con OLED"></div>

## 8. Errores comunes y variantes

| Síntoma | Causa típica | Solución |
|---|---|---|
| El cursor "?" no parpadea, permanece fijo | El `delay(100)` del bucle de `playhuman()` fue eliminado o modificado, alterando el muestreo de `millis()` contra `timerPos` | Conservar el `delay(100)` del bucle de sondeo; es necesario para el refresco correcto del display, según el propio comentario del autor original |
| El botón registra múltiples movimientos con una sola pulsación | Falta el bucle de debounce `while (digitalRead(BUTTON_MOVE)==LOW);` tras detectar la pulsación | Verificar que el bucle de debounce esté presente y no haya sido removido al adaptar el código |
| El display no responde | Bus I2C inicializado en pines que no corresponden al bloque I2C0/I2C1 real del montaje | Confirmar `Wire.setSDA()`/`Wire.setSCL()` contra la tabla de mapeo GPIO↔bloque I²C del RP2040 |
| La CPU siempre juega en la misma celda al inicio de cada partida | `randomSeed(analogRead(0))` fue removido, o el pin analógico usado como semilla está flotante en un valor constante | Verificar que `randomSeed()` se ejecute en `setup()` con un pin analógico sin conexión definida (ruido de piso como fuente de aleatoriedad) |
| El jugador logra vencer a la CPU de forma consistente | La heurística de la CPU (ver sección 4) no contempla dobles amenazas simultáneas; esto es una limitación conocida del algoritmo, no un error de implementación | No aplica corrección; es el comportamiento esperado de esta heurística de dos pasos |

**Variantes propuestas:**

1. Sustituir la heurística de bloqueo/victoria de dos pasos por un algoritmo
   Minimax completo, evaluando recursivamente todas las jugadas posibles
   hasta el final de la partida, para obtener una CPU matemáticamente
   invencible.
2. Añadir un marcador de partidas ganadas/perdidas/empatadas, almacenado en
   variables globales y mostrado en una franja adicional del display entre
   partidas.
3. Sustituir la selección de celda por dos botones (mover/confirmar) por un
   potenciómetro leído mediante ADC, mapeando su rango de lectura a las
   nueve celdas del tablero.

## Referencias

- Verrua, Giovanni. *TicTacToe - Oled* (código original para Arduino
  Uno/Nano/Mini Pro), abr/2022.
- Earle F. Philhower, III. *Arduino-Pico Documentation — I2C*.
  <https://arduino-pico.readthedocs.io/en/latest/i2c.html>
- Adafruit. *Adafruit SSD1306 library documentation*.
  <https://github.com/adafruit/Adafruit_SSD1306>
