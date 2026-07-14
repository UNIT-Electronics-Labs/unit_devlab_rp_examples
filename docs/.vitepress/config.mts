import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'RP2040 Labs',
  description: 'Documentación de ejemplos RP2040 con DevLab, C y Ensamblador.',
  lang: 'es-MX',
  base: process.env.BASE_PATH || '/',
  cleanUrls: true,
  themeConfig: {
    nav: [
      { text: 'Inicio', link: '/' },
      { text: 'Guías', link: '/guide/devlab' },
      { text: 'Ejemplos', link: '/examples/' },
      { text: 'Aplicaciones Arduino', link: '/arduino/' },
    ],
    sidebar: [
      {
        text: 'Primeros Pasos',
        items: [
          { text: 'DevLab', link: '/guide/devlab' },
          { text: 'Windows', link: '/guide/windows' },
          { text: 'Programación en C con Pico SDK', link: '/guide/c-programming' },
          { text: 'CMakeLists', link: '/guide/CMakeLists' },
          { text: 'Hardware Usado', link: '/guide/hardware' },
          { text: 'Arquitectura ARM', link: '/guide/arm' },
          { text: 'Compilación y Depuración', link: '/guide/openocd_pyocd' },
          { text: 'Programación en Ensamblador',
            collapsed: true,
            items: [
              { text: 'Registros', link: '/guide/asm/01_blinkraw' },
              { text: 'Branch and Link', link: '/guide/asm/02_blinkbl' },
              { text: 'UART', link: '/guide/asm/03_uarthello' },
            ]

          },
        ]
      },
      {
        text: 'Pico C SDK',
        items: [
          { text: 'Índice', link: '/examples/' },
          {
            text: 'Perifericos Basicos',
            collapsed: true,
            items: [
              {

                  text: 'Fundamentos',
                  items: [
                    { text: 'Blink: Hello World', link: '/examples/basic/00_blink' },
                    { text: 'Serial: Comunicación con PC', link: '/examples/basic/01_stdio' },
              ]
              },
              {

                  text: 'Entradas Digitales e Interrupciones',
                  items: [
                    { text: 'GPIO: Entradas y salidas', link: '/examples/basic/02_gpio' },
                    { text: 'IRQ: Interrupciones', link: '/examples/basic/03_irq' },
              ]
              },              
              {
                 
                  text: 'Señales Analogicas',
                  items: [
                    { text: 'ADC: Lectura Analógica', link: '/examples/basic/04_adc' },
                    { text: 'PWM: Control de Potencia', link: '/examples/basic/05_pwm' },
                    { text: 'Timer: Temporización', link: '/examples/basic/06_timer' },
              ]
              },
              {
                 
                  text: 'Protocolos de Comunicación',
                  items: [
                    { text: 'UART: Comunicación Serial', link: '/examples/basic/07_uart' },
                    { text: 'I2C: Comunicación en Bus', link: '/examples/basic/08_i2c' },
                    { text: 'SPI: Protocolo de Interface', link: '/examples/basic/09_spi' },
              ]
              },
              {
                 
                  text: 'Utilidades RP2040',
                  items: [
                    { text: 'Multicore: Uso de los dos núcleos', link: '/examples/basic/10_multicore' },
                    { text: 'PIO: Programable Input/Output', link: '/examples/basic/11_pio' },
              ]
              }
            ]
          },
          { text: 'Lectura de temperatura con OLED', link: '/examples/01_OledTempInt' },
          { text: 'PWM con ADC con OLED', link: '/examples/02_adcpwm' },
          { text: 'Sensor BMI270 SPI con OLED I2C', link: '/examples/03_oledbmi270' },
        ]
      },
      { 
        text: 'Arduino',
        items: [
          { text: 'Lectura de Sensor I2C con OLED', link: '/arduino/i2c_oled' },
          { text: 'Lectura de Sensor ADC', link: '/arduino/tmp235' },
          { text: 'Lectura de Sensor ADC Con OLED y UART', link: '/arduino/tmp235_oled' },
          { text: 'Interrupciones', link: '/arduino/interrupts' },
          { text: 'FlappyBird', link: '/arduino/OledFlappyBird' }
        ]
      }
    ],
    search: {
      provider: 'local'
    },
    outline: {
      level: [2, 3]
    },
    footer: {
      message: 'Ejemplos RP2040 para Raspberry Pi Pico.',
      copyright: 'MIT'
    }
  }
})
