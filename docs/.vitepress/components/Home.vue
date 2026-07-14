<!-- Test -->

<script setup>
import { computed } from 'vue'
import { withBase, useData } from 'vitepress'
import Cards from './Cards.vue'

const { isDark } = useData()

const skillsIcons = 'windows,linux,ubuntu,python,vscode,bash,powershell,cmake,c,cpp,arduino,raspberrypi'

const skillsSrc = computed(() => {
  return isDark.value
    ? `https://skillicons.dev/icons?i=${skillsIcons}`
    : `https://skillicons.dev/icons?i=${skillsIcons}&theme=light`
})

const featureCards = [
  {
    title: 'Arquitectura Cortex-M0+',
    description: 'Implementación práctica utilizando RP2040 como plataforma compatible con arquitectura ARM Cortex-M0+.',
  },
  {
    title: 'Ensamblador y C',
    description: 'Ejemplos en lenguaje ensamblador y C para entender la programación de bajo nivel en microcontroladores.',
  },
  {
    title: 'Periféricos completos',
    description: 'Prácticas con GPIO, UART, I²C, SPI, ADC y temporizadores del RP2040.',
  },
  {
    title: 'Herramientas de desarrollo',
    description: 'Flujo de desarrollo moderno con SDK oficial y herramientas open-source.',
  }
]
</script>

<template>
  <main class="home">
    <img
      class="wallpaper wallpaper-light"
      :src="withBase('/background.jpg')"
      alt=""
      aria-hidden="true"
    />

    <img
      class="wallpaper wallpaper-dark"
      :src="withBase('/background.jpg')"
      alt=""
      aria-hidden="true"
    />

    <section class="hero">
      <h1>
        RP2040 Labs
      </h1>

      <p class="description">
        Documentación práctica para programar el microcontrolador RP2040 en ensamblador y C, con ejemplos de comunicación UART, I²C, SPI, ADC y más.
      </p>

      <div class="actions">
        <a :href="withBase('/guide/devlab')" class="btn primary">
          Instalación
        </a>

        <a :href="withBase('/examples/')" class="btn secondary">
          Pico C SDK
        </a>
      </div>

      <div class="skills">
        <p>Funciona con las siguientes herramientas</p>

        <a class="skills-link">
            <img
            :key="skillsSrc"
            :src="skillsSrc"
            alt="Skills icons"
            />
        </a>
      </div>
    </section>
    <Cards :cards="featureCards" />
  </main>
</template>

<style scoped>
.home {
  position: relative;
  min-height: calc(100vh - 64px);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 96px 24px 88px;
  overflow: hidden;
  isolation: isolate;
}

.wallpaper {
  position: fixed;

  inset: -20px;

  width: calc(100vw + 40px);
  height: calc(100vh + 40px);

  object-fit: cover;
  object-position: center;

  filter: blur(12px);
  transform: scale(1.03);

  pointer-events: none;
  user-select: none;
  z-index: -1;
}

.wallpaper-light {
  display: block;
}

.wallpaper-dark {
  display: none;
}

.dark .wallpaper-light {
  display: none;
}

.dark .wallpaper-dark {
  display: block;
}

.hero {
  position: relative;
  z-index: 2;
  width: 100%;
  max-width: 760px;
  text-align: center;
}

h1 {
  max-width: 760px;
  margin: 0 auto;
  color: #000000;
  font-size: clamp(34px, 6vw, 64px);
  line-height: 1.05;
  letter-spacing: -0.04em;
  font-weight: 800;
}

.description {
  max-width: 620px;
  margin: 24px auto 0;
  color: #3a3a3a;
  font-size: 19px;
  line-height: 1.65;
  font-weight: 450;
}

.actions {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 14px;
  margin-top: 34px;
  flex-wrap: wrap;
}

.btn {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  min-height: 42px;
  padding: 0 22px;
  border-radius: 999px;
  font-size: 14px;
  font-weight: 700;
  text-decoration: none;
  transition:
    transform 0.18s ease,
    background-color 0.18s ease,
    border-color 0.18s ease;
}

.btn:hover {
  transform: translateY(-2px);
}

.primary {
  color: white;
  background-color: #005999;
}

.primary:hover {
  background-color: #014779;
}

.secondary {
  color: #3a3a3a;
  background-color: #ffffff;
  border: 1px solid #ffffff;
}

.secondary:hover {
  border-color: #014779;
}

.skills {
  margin-top: 72px;
}

.skills p {
  margin: 0 0 18px;
  color: #ffffff;
  font-size: 13px;
  font-weight: 450;
}

.skills-link {
  display: inline-flex;
  justify-content: center;
  align-items: center;
}

.skills img {
  display: block;
  max-width: 100%;
  height: auto;
}

@media (max-width: 640px) {
  .home {
    min-height: calc(100vh - 64px);
    padding: 72px 20px;
  }

  h1 {
    font-size: clamp(36px, 12vw, 48px);
  }

  .description {
    font-size: 17px;
  }

  .skills {
    margin-top: 56px;
  }

  .wallpaper-light {
    opacity: 0.06;
  }

  .wallpaper-dark {
    opacity: 0.24;
  }
}
</style>