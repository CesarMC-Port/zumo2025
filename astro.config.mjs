// @ts-check
import { defineConfig } from 'astro/config';
import node from '@astrojs/node';

import sitemap from "@astrojs/sitemap";
import lottie from "astro-integration-lottie";
import vue from '@astrojs/vue';
import tailwind from '@astrojs/tailwind';
import relativeLinks from 'astro-relative-links';

export default defineConfig({
  site: 'https://cesarmc-port.github.io',
  base: '/zumo2025',
  integrations: [vue(), sitemap({
    serialize(item) {
      if (item.url === 'https://cesarmc-port.github.io/zumo2025') {
        // @ts-ignore: conflicto sitemap type changefreq 
        item.changefreq = 'daily';
        item.lastmod = '2024-12-26'; 
        item.priority = 1.0;
      }

      return item;
    },
  }), lottie(), tailwind(), relativeLinks()],
  output: 'static',
  adapter: node({ mode: 'standalone' })
});