# BMI270 por SPI + OLED por I2C: aceleracion y giro en pantalla

Esta aplicacion integra dos periféricos ya cubiertos por separado en Periféricos Básicos —el bus SPI (práctica 09) y el controlador OLED SSD1306 (aplicación 01, vía I2C)— para construir un caso de uso real: un sensor inercial de 6 ejes cuyas lecturas se despliegan en una pantalla, sin necesidad de consola serial para operar. Este patrón —sensor + display, sin depender de una PC— es la base de la gran mayoría de instrumentos de medición embebidos: estaciones meteorológicas, controladores de vuelo, wearables de actividad física, entre otros.

## Concepto teórico

El BMI270 es un sensor inercial (IMU) de 6 grados de libertad: un acelerómetro y un giroscopio MEMS integrados en un mismo encapsulado, cada uno con su propia cadena de conversión analógico-digital interna. A diferencia de un sensor simple (como un termistor), el BMI270 no entrega datos utilizables inmediatamente después del encendido: su lógica interna requiere que el host le transfiera un archivo de configuración de 8192 bytes (provisto por el fabricante) antes de que los registros de datos comiencen a poblarse con mediciones válidas. Este archivo no contiene información específica de la aplicación; es firmware genérico que habilita las rutas de procesamiento internas del sensor (filtros, compensación de temperatura, etc.), común a cualquier proyecto que use este chip.

Sobre el bus SPI, el BMI270 tiene una particularidad de trama ya documentada en la práctica 09: cada lectura de registro requiere un byte de dirección (bit más significativo en 1) seguido de un byte *dummy* adicional antes de que el dato real aparezca en el bus. Esto es distinto de una lectura SPI genérica de un solo byte de por medio, y es la causa más común de lecturas erróneas al implementar este sensor desde cero.

Los datos de aceleración y giro se transmiten como enteros con signo de 16 bits (formato *LSB primero*), uno por eje, en un bloque contiguo de 12 bytes a partir del registro `0x0C`. La conversión a unidades físicas depende del rango configurado: con un rango de ±2 g para el acelerómetro, el valor de fondo de escala (32768 cuentas) equivale a 2 g, de modo que 1 cuenta = 2/32768 g; el mismo principio aplica al giroscopio con su rango en grados por segundo.

## Hardware y Conexiones

| Señal | Periférico | Pin RP2040 | Notas |
|---|---|---|---|
| SDA | OLED SSD1306 (I2C0) | GP0 | Pull-up interno habilitado por software |
| SCL | OLED SSD1306 (I2C0) | GP1 | Pull-up interno habilitado por software |
| SCK | BMI270 (SPI0) | GP2 | Reloj, modo 0 (CPOL=0, CPHA=0) |
| MOSI | BMI270 (SPI0) | GP3 | Etiquetado "SDA" en el módulo BMI270 |
| MISO | BMI270 (SPI0) | GP4 | Etiquetado "SDO" en el módulo BMI270 |
| CS | BMI270 (SPI0) | GP5 | Control manual por GPIO, activo en bajo |

**Nota:** en el módulo BMI270 del UNIT DevLab MultiHub Shield, los pines físicos conservan las etiquetas de su modo I2C (SDA, SCL) incluso cuando se usan por SPI, donde corresponden a MOSI y SCK respectivamente. El puente de soldadura SB1 del módulo (selector de modo I2C/SPI3/SPI4) puede permanecer sin soldar: la selección de interfaz se resuelve por software mediante una lectura SPI descartable al inicio.

## Dependencias de software

| Librería | Autor | Licencia | Repositorio | Uso en esta aplicación |
|---|---|---|---|---|
| `pico-ssd1306` | David Schramm (daschr) | MIT | github.com/daschr/pico-ssd1306 | Inicialización del controlador SSD1306, framebuffer y texto |
| Archivo de configuración BMI270 (`bmi270_config_file`) | Bosch Sensortec GmbH | BSD-3-Clause | github.com/boschsensortec/BMI270_SensorAPI | Firmware interno de 8192 bytes, obligatorio para que el sensor entregue datos de aceleración y giro |

El código fuente de ambas dependencias no se reproduce en esta documentación; se referencian por nombre, autor, licencia y repositorio de origen. El archivo de configuración del BMI270 se incluye en el proyecto como `bmi270_config.h`, generado una única vez a partir del driver oficial de Bosch, sin modificaciones sobre los datos.

## Configuración del Proyecto — CMake

```cmake
add_executable(app02_bmi270_oled
    main.c
    lib/ssd1306/ssd1306.c
)

target_include_directories(app02_bmi270_oled PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib/ssd1306
)

target_link_libraries(app02_bmi270_oled
    pico_stdlib
    hardware_i2c
    hardware_spi
)

pico_add_extra_outputs(app02_bmi270_oled)
```

`bmi270_config.h` no requiere entrada propia en `PROJECT_SOURCES`: es un header de solo datos, incluido directamente desde `main.c`.

## Código Fuente

```c
/**
 * @file main.c
 * @brief Proyecto 13_OLEDSPIBMI270_Application para Raspberry Pi Pico
 *
 * Integracion del sensor inercial BMI270 (SPI) con la pantalla OLED
 * SSD1306 (I2C): se inicializa el sensor con su archivo de configuracion
 * oficial, se leen aceleracion y velocidad angular en los tres ejes, y
 * los valores se despliegan en la pantalla y en la consola serial.
 *
 * @author obviousfancy
 * @date 2026-07-13
 *
 * @board pico
 * @sdk Raspberry Pi Pico SDK 2.2.0
 */

/* ─── Includes ─────────────────────────────────────────── */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "hardware/spi.h"
#include "bmi270_config.h"   // Archivo de configuracion oficial de Bosch (8192 bytes, BSD-3-Clause)

/* ─── Defines ──────────────────────────────────────────── */
// ─── Configuraciones I2C y OLED ────────────────────────
#define I2C_PORT i2c0
#define I2C_SDA 0 // Pin GP0
#define I2C_SCL 1 // Pin GP1
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// ─── Configuraciones SPI y BMI270 ──────────────────────
#define SPI_PORT   spi0
#define PIN_SCK    2
#define PIN_MOSI   3
#define PIN_MISO   4
#define PIN_CS     5

// Registros del BMI270 utilizados en esta aplicacion
#define BMI270_REG_CHIP_ID          0x00
#define BMI270_REG_DATA_ACC_X_LSB   0x0C  // Inicio del bloque de datos: ACC XYZ + GYR XYZ (12 bytes)
#define BMI270_REG_INTERNAL_STATUS  0x21
#define BMI270_REG_ACC_CONF         0x40
#define BMI270_REG_ACC_RANGE        0x41
#define BMI270_REG_GYR_CONF         0x42
#define BMI270_REG_GYR_RANGE        0x43
#define BMI270_REG_INIT_CTRL        0x59
#define BMI270_REG_INIT_ADDR_0      0x5B
#define BMI270_REG_INIT_ADDR_1      0x5C
#define BMI270_REG_INIT_DATA        0x5E
#define BMI270_REG_CMD              0x7E
#define BMI270_REG_PWR_CONF         0x7C
#define BMI270_REG_PWR_CTRL         0x7D

#define BMI270_CHIP_ID_VALUE        0x24
#define BMI270_CMD_SOFT_RESET       0xB6

// Rangos configurados y sus factores de conversion.
// Los datos crudos son enteros con signo de 16 bits: el fondo de escala
// corresponde a 32768 cuentas, por lo que 1 cuenta = rango / 32768.
#define ACC_RANGE_G      2.0f     // ACC_RANGE = 0x00 -> +/-2 g
#define GYR_RANGE_DPS    2000.0f  // GYR_RANGE = 0x00 -> +/-2000 grados/s

/* ─── Funciones de acceso al BMI270 por SPI ────────────── */

// Lectura de un solo registro. El BMI270 requiere, en cada lectura por
// SPI: byte de direccion (con bit 7 en 1), un byte dummy adicional, y
// recien despues el dato real. El dato valido llega en rx_buf[2].
static uint8_t bmi270_leer_registro(uint8_t reg) {
    uint8_t tx_buf[3] = { reg | 0x80, 0x00, 0x00 };
    uint8_t rx_buf[3] = { 0 };

    gpio_put(PIN_CS, 0);
    spi_write_read_blocking(SPI_PORT, tx_buf, rx_buf, 3);
    gpio_put(PIN_CS, 1);

    return rx_buf[2];
}

// Lectura en rafaga de varios registros consecutivos. El byte dummy
// aparece una sola vez por transaccion (tras el byte de direccion);
// los datos siguientes llegan de forma continua.
static void bmi270_leer_rafaga(uint8_t reg, uint8_t *datos, size_t n) {
    uint8_t tx = reg | 0x80;
    uint8_t descarte;

    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &tx, 1);        // direccion
    spi_read_blocking(SPI_PORT, 0x00, &descarte, 1); // byte dummy
    spi_read_blocking(SPI_PORT, 0x00, datos, n);     // datos reales
    gpio_put(PIN_CS, 1);
}

// Escritura de un solo registro. En escritura no existe byte dummy:
// direccion (bit 7 en 0) seguida inmediatamente del dato.
static void bmi270_escribir_registro(uint8_t reg, uint8_t valor) {
    uint8_t tx_buf[2] = { reg, valor };

    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, tx_buf, 2);
    gpio_put(PIN_CS, 1);
}

// Escritura en rafaga (usada para cargar el archivo de configuracion).
static void bmi270_escribir_rafaga(uint8_t reg, const uint8_t *datos, size_t n) {
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &reg, 1);
    spi_write_blocking(SPI_PORT, datos, n);
    gpio_put(PIN_CS, 1);
}

/* ─── Inicializacion del BMI270 ────────────────────────── */

// Secuencia de arranque conforme al datasheet de Bosch (seccion de
// inicializacion). El sensor no produce datos de aceleracion ni giro
// hasta completar la carga del archivo de configuracion de 8192 bytes.
// Devuelve true si la inicializacion fue exitosa.
static bool bmi270_init(void) {
    // 1. Lectura dummy de CHIP_ID: la primera transaccion por SPI tras el
    //    encendido activa la interfaz SPI del sensor (arranca en modo I2C).
    (void)bmi270_leer_registro(BMI270_REG_CHIP_ID);
    sleep_ms(1);

    // 2. Soft reset para partir de un estado conocido. Tras el reset el
    //    sensor vuelve a modo I2C, por lo que se repite la lectura dummy.
    bmi270_escribir_registro(BMI270_REG_CMD, BMI270_CMD_SOFT_RESET);
    sleep_ms(2);
    (void)bmi270_leer_registro(BMI270_REG_CHIP_ID);
    sleep_ms(1);

    // 3. Verificacion de identidad.
    uint8_t chip_id = bmi270_leer_registro(BMI270_REG_CHIP_ID);
    if (chip_id != BMI270_CHIP_ID_VALUE) {
        printf("BMI270: CHIP_ID incorrecto (0x%02X, esperado 0x%02X)\n",
               chip_id, BMI270_CHIP_ID_VALUE);
        return false;
    }

    // 4. Deshabilitar el modo de ahorro avanzado de energia; el sensor
    //    arranca con el en 1 y en ese estado la carga de configuracion
    //    no es posible. Espera minima de 450 us tras el cambio.
    bmi270_escribir_registro(BMI270_REG_PWR_CONF, 0x00);
    sleep_us(500);

    // 5. Preparar la carga: INIT_CTRL = 0x00.
    bmi270_escribir_registro(BMI270_REG_INIT_CTRL, 0x00);

    // 6. Cargar el archivo de configuracion en bloques. Antes de cada
    //    bloque se escribe la direccion destino (en palabras de 16 bits)
    //    en INIT_ADDR_0 (4 bits bajos) e INIT_ADDR_1 (8 bits altos).
    const size_t TAM_BLOQUE = 256;
    for (size_t offset = 0; offset < sizeof(bmi270_config_file); offset += TAM_BLOQUE) {
        uint16_t addr_palabras = (uint16_t)(offset / 2);
        bmi270_escribir_registro(BMI270_REG_INIT_ADDR_0, addr_palabras & 0x0F);
        bmi270_escribir_registro(BMI270_REG_INIT_ADDR_1, (addr_palabras >> 4) & 0xFF);
        bmi270_escribir_rafaga(BMI270_REG_INIT_DATA,
                               &bmi270_config_file[offset], TAM_BLOQUE);
    }

    // 7. Finalizar la carga: INIT_CTRL = 0x01. Esta escritura no debe
    //    realizarse mas de una vez por ciclo de encendido o soft reset.
    bmi270_escribir_registro(BMI270_REG_INIT_CTRL, 0x01);

    // 8. Esperar a que INTERNAL_STATUS reporte inicializacion correcta
    //    (message = 0b0001). El datasheet indica un maximo de 20 ms;
    //    se sondea con margen amplio.
    bool listo = false;
    for (int i = 0; i < 100; i++) {
        uint8_t estado = bmi270_leer_registro(BMI270_REG_INTERNAL_STATUS);
        if ((estado & 0x0F) == 0x01) {
            listo = true;
            break;
        }
        sleep_ms(1);
    }
    if (!listo) {
        printf("BMI270: la carga de configuracion no finalizo correctamente\n");
        return false;
    }

    // 9. Habilitar acelerometro, giroscopio y sensor de temperatura
    //    (PWR_CTRL = 0x0E). El bit de interfaz auxiliar permanece en 0:
    //    no hay sensor externo conectado al hub del BMI270.
    bmi270_escribir_registro(BMI270_REG_PWR_CTRL, 0x0E);

    // 10. Configuracion de ambos sensores en modo de rendimiento:
    //     ACC_CONF = 0xA8 -> ODR 100 Hz, filtro en modo rendimiento.
    //     GYR_CONF = 0xA9 -> ODR 200 Hz, filtro en modo rendimiento.
    bmi270_escribir_registro(BMI270_REG_ACC_CONF, 0xA8);
    bmi270_escribir_registro(BMI270_REG_GYR_CONF, 0xA9);

    // 11. Rangos explicitos, para que la conversion a unidades fisicas
    //     sea inequivoca: +/-2 g y +/-2000 grados/s.
    bmi270_escribir_registro(BMI270_REG_ACC_RANGE, 0x00);
    bmi270_escribir_registro(BMI270_REG_GYR_RANGE, 0x00);

    // 12. PWR_CONF = 0x02: ahorro avanzado deshabilitado (modo de
    //     rendimiento continuo), fifo_self_wake_up habilitado (valor
    //     recomendado por el procedimiento de arranque del fabricante).
    bmi270_escribir_registro(BMI270_REG_PWR_CONF, 0x02);
    sleep_ms(10);

    return true;
}

// Lectura de los 6 valores crudos (ACC XYZ + GYR XYZ) en una sola rafaga
// de 12 bytes a partir de 0x0C. Cada valor es de 16 bits, LSB primero.
static void bmi270_leer_datos(int16_t acc[3], int16_t gyr[3]) {
    uint8_t crudo[12];
    bmi270_leer_rafaga(BMI270_REG_DATA_ACC_X_LSB, crudo, sizeof(crudo));

    acc[0] = (int16_t)((crudo[1] << 8) | crudo[0]);
    acc[1] = (int16_t)((crudo[3] << 8) | crudo[2]);
    acc[2] = (int16_t)((crudo[5] << 8) | crudo[4]);
    gyr[0] = (int16_t)((crudo[7] << 8) | crudo[6]);
    gyr[1] = (int16_t)((crudo[9] << 8) | crudo[8]);
    gyr[2] = (int16_t)((crudo[11] << 8) | crudo[10]);
}

/* ─── Main ─────────────────────────────────────────────── */
int main() {
    // CS se configura y se fuerza a bajo antes de cualquier otra
    // inicializacion, en linea con la recomendacion del fabricante del
    // modulo sobre la seleccion de interfaz del BMI270 al energizarse.
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 0);

    stdio_init_all();

    // Retardo de arranque: da tiempo a que el host establezca la conexion
    // USB CDC antes de emitir cualquier printf.
    sleep_ms(10000);

    // 1. Inicializar I2C (400 kHz permite un refresco agil de la pantalla)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // 2. Inicializar SPI a 1 MHz, modo 0 (CPOL=0, CPHA=0), MSB primero
    uint baud_real = spi_init(SPI_PORT, 1000 * 1000);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    // CS ya se inicializo como salida al inicio de main(); configurado el
    // resto del bus, se libera a su nivel de reposo (alto, activo en bajo).
    gpio_put(PIN_CS, 1);

    printf("Frecuencia SPI solicitada: 1000000 Hz, real: %u Hz\n", baud_real);

    // 3. Inicializar la pantalla OLED
    ssd1306_t display;
    // CRITICO: configurar external_vcc a false antes del init; de lo
    // contrario la bomba de carga no enciende y la pantalla queda negra.
    display.external_vcc = false;

    bool oled_ok = ssd1306_init(&display, OLED_WIDTH, OLED_HEIGHT, OLED_ADDR, I2C_PORT);
    if (!oled_ok) {
        printf("Fallo al inicializar la pantalla OLED. Revisa las conexiones.\n");
        while (1) sleep_ms(1000);
    }
    printf("Pantalla OLED inicializada correctamente.\n");

    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 0, 0, 1, "Iniciando BMI270...");
    ssd1306_show(&display);

    // 4. Inicializar el BMI270 (carga de configuracion incluida)
    if (!bmi270_init()) {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Error BMI270");
        ssd1306_draw_string(&display, 0, 16, 1, "Revisa conexiones");
        ssd1306_show(&display);
        while (1) sleep_ms(1000);
    }
    printf("BMI270 inicializado correctamente.\n");

    // 5. Bucle principal: leer, convertir, desplegar
    int16_t acc_crudo[3], gyr_crudo[3];
    char linea[24];

    while (1) {
        bmi270_leer_datos(acc_crudo, gyr_crudo);

        // Conversion a unidades fisicas segun los rangos configurados
        float ax = acc_crudo[0] * ACC_RANGE_G / 32768.0f;
        float ay = acc_crudo[1] * ACC_RANGE_G / 32768.0f;
        float az = acc_crudo[2] * ACC_RANGE_G / 32768.0f;
        float gx = gyr_crudo[0] * GYR_RANGE_DPS / 32768.0f;
        float gy = gyr_crudo[1] * GYR_RANGE_DPS / 32768.0f;
        float gz = gyr_crudo[2] * GYR_RANGE_DPS / 32768.0f;

        // Despliegue en OLED: acelerometro arriba, giroscopio abajo.
        // Fuente base 5x8: escala 1 permite ~21 caracteres por linea.
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Acel [g]");
        snprintf(linea, sizeof(linea), "X%+.2f Y%+.2f", ax, ay);
        ssd1306_draw_string(&display, 0, 10, 1, linea);
        snprintf(linea, sizeof(linea), "Z%+.2f", az);
        ssd1306_draw_string(&display, 0, 20, 1, linea);

        ssd1306_draw_string(&display, 0, 34, 1, "Giro [dps]");
        snprintf(linea, sizeof(linea), "X%+.1f Y%+.1f", gx, gy);
        ssd1306_draw_string(&display, 0, 44, 1, linea);
        snprintf(linea, sizeof(linea), "Z%+.1f", gz);
        ssd1306_draw_string(&display, 0, 54, 1, linea);
        ssd1306_show(&display);

        // Despliegue en consola serial
        printf("Acel [g]: X=%+.3f Y=%+.3f Z=%+.3f | Giro [dps]: X=%+.2f Y=%+.2f Z=%+.2f\n",
               ax, ay, az, gx, gy, gz);

        // ~10 Hz de refresco: suficiente para visualizacion y muy por
        // debajo del costo de refresco I2C del OLED (~23 ms por cuadro).
        sleep_ms(100);
    }
    return 0;
}
```

## Análisis del Código

Elementos introducidos por esta aplicación, no vistos en la práctica 09 de SPI ni en la aplicación 01 del OLED:

- **`bmi270_leer_rafaga()` y `bmi270_escribir_rafaga()`**: a diferencia de la lectura/escritura de un solo registro, la práctica 09 solo cubrió transacciones de un byte. Aquí, la lectura en ráfaga permite obtener los 12 bytes del bloque de datos (aceleración + giro) en una sola transacción de bus, en vez de 12 lecturas independientes de registro. El byte *dummy* aparece una única vez, inmediatamente después de la dirección, no una vez por cada byte de dato.
- **`bmi270_init()`**: implementa la secuencia oficial de arranque del fabricante (deshabilitar ahorro de energía, cargar el archivo de configuración en bloques direccionados, verificar `INTERNAL_STATUS`, habilitar acelerómetro y giroscopio con sus rangos). Ningún paso de esta secuencia es opcional: omitir cualquiera deja al sensor reportando ceros o valores inconsistentes, incluso con la comunicación SPI funcionando correctamente.
- **Direccionamiento por bloques del `INIT_DATA`**: el archivo de 8192 bytes se transfiere en fragmentos de 256 bytes (no en una sola ráfaga) porque el registro de dirección interno del BMI270 (`INIT_ADDR_0`/`INIT_ADDR_1`) opera en unidades de palabras de 16 bits, y debe reprogramarse antes de cada bloque.
- **Conversión de datos crudos a unidades físicas**: los 12 bytes leídos se reinterpretan como 6 enteros de 16 bits con signo (LSB primero), y cada uno se escala según el rango configurado (`cuenta × rango_fondo_escala / 32768`).

## Verificación

**Consola serial**, tras el arranque:

```
Frecuencia SPI solicitada: 1000000 Hz, real: 992063 Hz
Pantalla OLED inicializada correctamente.
BMI270 inicializado correctamente.
Acel [g]: X=+0.024 Y=-0.004 Z=+0.996 | Giro [dps]: X=-0.061 Y=+1.160 Z=-0.183
```

**Pantalla OLED**, actualizada aproximadamente 10 veces por segundo:

```
Acel [g]
X+0.02 Y-0.00
Z+0.99

Giro [dps]
X-0.1 Y+1.2
Z-0.2
```

Con la placa en reposo sobre una superficie plana, el eje Z del acelerómetro debe aproximarse a +1.00 g (gravedad) y los demás valores deben mantenerse cercanos a cero, con ruido menor a ±0.05 g y ±2 dps. Al inclinar o mover la placa, los valores deben responder en tiempo real, sin retraso perceptible.

<div align="center"><img src="/resources/app02_bmi270_oled_verificacion.png" alt="Pantalla OLED mostrando lecturas de aceleracion y giro del BMI270"></div>

**Cálculo de margen temporal:** una ráfaga de adquisición (1 byte de dirección + 1 byte dummy + 12 bytes de datos = 14 bytes) a 992063 Hz toma aproximadamente 113 µs. El refresco completo del OLED por I2C a 400 kHz (128×64 monocromo) toma aproximadamente 23 ms, tal como se documentó en la aplicación 01. Frente a un período de bucle de 100 ms, el margen disponible es de aproximadamente 4.3×, suficiente para sostener el refresco de pantalla sin acumular retraso.

## Errores Comunes y Variantes

| Síntoma | Causa típica |
|---|---|
| `CHIP_ID incorrecto` al inicio | Cableado SPI incorrecto, o lectura de registro sin el byte dummy (framing de 2 bytes en vez de 3) |
| `la carga de configuracion no finalizo correctamente` | Bloques de `INIT_DATA` escritos en orden incorrecto, o `INIT_ADDR_0`/`INIT_ADDR_1` no actualizados antes de cada bloque |
| Aceleración y giro permanecen en 0.000 tras una inicialización "exitosa" | `PWR_CTRL` no habilitó acelerómetro/giroscopio (paso 9), o el archivo de configuración cargado está incompleto o corrupto |
| Pantalla OLED en negro, consola con lecturas correctas | `external_vcc` no configurado en `false` antes de `ssd1306_init()` (ver aplicación 01) |
| Valores de aceleración plausibles pero giro con deriva pronunciada | Comportamiento esperado sin calibración de offset (`performGyroOffsetCalibration()` en la librería SparkFun); fuera del alcance de esta aplicación introductoria |

**Variantes propuestas:**

1. Agregar una tercera línea de texto que muestre la magnitud total de aceleración (`sqrt(ax² + ay² + az²)`), útil para detectar impactos o caídas libres.
2. Cambiar el rango del acelerómetro a ±4 g o ±8 g (`ACC_RANGE` y `ACC_RANGE_G`) y verificar el efecto sobre la resolución de las lecturas en reposo.
3. Sustituir el refresco por tiempo fijo (`sleep_ms(100)`) por un refresco condicionado a `getStatus()`/bit de datos listos, evitando leer el mismo dato dos veces si el sensor aún no actualizó su registro.
