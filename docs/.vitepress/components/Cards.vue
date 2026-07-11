<script setup>
defineProps({
  cards: {
    type: Array,
    required: true
  }
})
</script>

<template>
  <section
    class="cards-section"
    aria-label="Características principales"
  >
    <article
      v-for="card in cards"
      :key="card.title"
      class="feature-card"
    >
      <h2>{{ card.title }}</h2>
      <p>{{ card.description }}</p>
    </article>
  </section>
</template>

<style scoped>
.cards-section {
  width: 100%;
  max-width: 1120px;
  margin: 72px auto 0;
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 16px;
}

.feature-card {
  min-height: 160px;
  padding: 28px;
  border-radius: 16px;
  background: rgb(247, 247, 247);
  border: 1px solid transparent;
  text-decoration: none;
  backdrop-filter: blur(12px);
  transition:
    transform 0.18s ease,
    border-color 0.18s ease,
    background-color 0.18s ease;
}

.feature-card:hover {
  transform: translateY(-4px);
  border-color: #014779;
}

.feature-card h2 {
  margin: 0;
  color: var(--vp-c-text-1);
  font-size: 18px;
  font-weight: 600;
  letter-spacing: -0.02em;
}

.feature-card p {
  margin: 4px 0 0;
  color: var(--vp-c-text-2);
  font-size: 15.5px;
  line-height: 1.65;
  font-weight: 450;
}

.dark .feature-card {
  background: rgba(22, 22, 24, 0.72);
  border-color: rgba(255, 255, 255, 0.08);
}

.dark .feature-card:hover {
  border-color: #014779;
  background: rgba(28, 28, 31, 0.82);
}

@media (max-width: 1024px) {
  .cards-section {
    grid-template-columns: repeat(2, minmax(0, 1fr));
    max-width: 760px;
  }
}

@media (max-width: 640px) {
  .cards-section {
    grid-template-columns: 1fr;
    margin-top: 56px;
  }

  .feature-card {
    min-height: auto;
    padding: 24px;
  }
}
</style>