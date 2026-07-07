import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'Gowin FPGA Labs',
  description: 'Documentación de ejemplos FPGA Gowin con DevLab, Verilog y VHDL.',
  lang: 'es-MX',
  cleanUrls: true,
  themeConfig: {
    nav: [
      { text: 'Inicio', link: '/' },
      { text: 'Guías', link: '/guide/devlab' },
      { text: 'Ejemplos', link: '/examples/' },
      { text: 'Digital Labs', link: '/examples/digital-labs/' }
    ],
    sidebar: [
      {
        text: 'Primeros Pasos',
        items: [
          { text: 'DevLab', link: '/guide/devlab' },
          { text: 'Windows', link: '/guide/windows' },
          { text: 'Verilog', link: '/guide/verilog' },
          { text: 'VHDL', link: '/guide/vhdl' },
          { text: 'Archivos CST', link: '/guide/cst' }
        ]
      },
      {
        text: 'Ejemplos Básicos',
        items: [
          { text: 'Índice', link: '/examples/' },
          { text: 'Blink', link: '/examples/basic/blink' },
          { text: 'AND', link: '/examples/basic/and' },
          { text: 'OR', link: '/examples/basic/or' },
          { text: 'NOT', link: '/examples/basic/not' }
        ]
      },
      {
        text: 'Digital Labs',
        items: [
          { text: 'Índice', link: '/examples/digital-labs/' },
          { text: '01 Sumador N bits', link: '/examples/digital-labs/01_sumador_n_bits' },
          { text: '02 Restador N bits', link: '/examples/digital-labs/02_restador_n_bits' },
          { text: '03 Comparador N bits', link: '/examples/digital-labs/03_comparador_n_bits' },
          { text: '04 Decoder 7 segmentos', link: '/examples/digital-labs/04_decodificador_7_segmentos' },
          { text: '05 Decoder 16 segmentos', link: '/examples/digital-labs/05_decodificador_16_segmentos' },
          { text: '06 Restador a 7 segmentos', link: '/examples/digital-labs/06_restador_a_7_segmentos' },
          { text: '07 ALU N bits', link: '/examples/digital-labs/07_alu_n_bits' },
          { text: '08 Señal de reloj', link: '/examples/digital-labs/08_senal_reloj' },
          { text: '09 Contador ascendente', link: '/examples/digital-labs/09_contador_ascendente' },
          { text: '10 Contador descendente', link: '/examples/digital-labs/10_contador_descendente' },
          { text: '11 Contador up/down', link: '/examples/digital-labs/11_contador_ascendente_descendente' },
          { text: '12 Arranque paro reset', link: '/examples/digital-labs/12_contador_arranque_paro_reset' },
          { text: '13 Máquina Moore', link: '/examples/digital-labs/13_maquina_moore' },
          { text: '14 Máquina Mealy', link: '/examples/digital-labs/14_maquina_mealy' }
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
      message: 'Ejemplos FPGA para Tang Nano 9K.',
      copyright: 'MIT'
    }
  }
})
