<template>
  <div class="simulator-container">
    <div class="canvas-wrapper">
      <canvas id="arena" width="800" height="800"></canvas>
    </div>

    <div class="controls">
      <p class="text-xl text-center">Click para soltar una caja</p>
      <button id="reset-btn">Reset</button>
    </div>
  </div>
</template>

<script setup lang="ts">
    import { onMounted } from "vue";
    import { initSimulator } from "../scripts.ts";

    onMounted(() => {
      initSimulator();
    });
</script>

<style scoped>
.simulator-container {
  position: relative;
  width: 100%;
  height: 100%;
  padding: 60px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  transform: scale(1);
}

.canvas-wrapper {
  transform: scale(0.9);
}

#arena {
  background: transparent;
  border-radius: 8px;
  margin-top: 20px;
  display: block;
}

.controls {
  transform: translateY(-80px);
  margin-top: 10px;
  display: flex;
  justify-content: center;
  align-items: center;
  flex-direction: column;
  gap: 20px;
}

button {
  padding: 8px 14px;
  font-size: 14px;
  border-radius: 6px;
  cursor: pointer;
  border: none;
  background: #555;
  color: #fff;
}

button:hover {
  background: #777;
}
</style>
