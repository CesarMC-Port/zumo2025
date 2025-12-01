<template>
    <div ref="wrapper" class="spline-wrapper">
        <div class="spline-side">
            <div class="spline-loader" v-if="loading">
                <div class="loaderSpline"></div>
                <div class="message">Cargando escena...</div>
            </div>
        
            <iframe
                v-if="shouldLoad"
                :src="splineSrc"
                @load="loading = false"
            ></iframe> 
            
        </div>
    </div>
</template>

<script setup>
    import { ref, onMounted } from "vue";

    const shouldLoad = ref(false);
    const loading = ref(false);
    const wrapper = ref(null);

    const splineSrc = "https://my.spline.design/untitled-Wx0LflKukFCFX7d4PLOoK5jG/";

    onMounted(() => {
    const observer = new IntersectionObserver(
        (entries) => {
        if (entries[0].isIntersecting && !shouldLoad.value) {
            shouldLoad.value = true;
            loading.value = true;
            observer.disconnect();
        }
        },
        { threshold: 0.3 }
    );

    observer.observe(wrapper.value);
    });
</script>

<style scoped>
    .spline-wrapper {
        position: fixed;
        right: 0px;
        top: 0;
        width: 100%;
        height: calc(100vh - 60px);
        z-index: 100;
        background: transparent;
    }

    .spline-side{
        position: relative;
        width: 100%;
        height: calc(100vh - 60px);
    }

    iframe {
        position: absolute;
        right: 0;
        top: 0;
        width: 100%;
        height: calc(100vh - 60px);
        z-index: 5;
        clip-path: polygon(0 0, 100% 0, 100% 92.3%, 68% 92.3%, 68% 100%, 0 100%);
    }
    .spline-loader {
        position: absolute;
        inset: 0;
        display: flex;
        align-items: center;
        justify-content: center;
        flex-direction: column;
        gap: 5px;
        color: #fff;
        z-index: 20;
        transition: opacity 400ms ease, visibility 400ms;
        height: 50%;
    }

    .spline-loader .message {
        font-size: 16px;
    }
</style>