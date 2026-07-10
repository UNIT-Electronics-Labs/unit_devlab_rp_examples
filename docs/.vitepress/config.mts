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
          { text: 'C Programming', link: '/guide/c-programming' },
          { text: 'CMakeLists', link: '/guide/CMakeLists' },
          { text: 'Hardware Usado', link: '/guide/hardware' }
        ]
      },
      {
        text: 'Ejemplos Básicos',
        items: [
          { text: 'Índice', link: '/examples/' },
          { text: 'Blink', link: '/examples/basic/blink' },
          { text: 'GPIO', link: '/examples/basic/gpio' },
          { text: 'ADC', link: '/examples/basic/adcread' },
          { text: 'UART', link: '/examples/basic/uart' },
          { text: 'OLED', link: '/examples/basic/oled' },
          { text: 'I2C con OLED', link: '/examples/i2c_oled' },
          { text: 'TMP235 ADC', link: '/examples/adcTMP235' },
          { text: 'TMP235 ADC con OLED', link: '/examples/oled_tmp235' }
        ]
      },
      { 
        text: 'Aplicaciones Arduino',
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
