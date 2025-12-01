import defaultTheme from 'tailwindcss/defaultTheme'

/** @type {import('tailwindcss').Config} */
export default {
	content: ['./src/**/*.{astro,html,js,jsx,md,mdx,svelte,ts,tsx,vue,json}'],
	theme: {
		extend: {
		  fontFamily: {
			Rubik: ['Rubik', ...defaultTheme.fontFamily.sans],
			sans: ['Inter', 'system-ui', 'sans-serif'],
        	tech: ['Space Grotesk', 'system-ui', 'sans-serif'],
		  },
		  colors: {
			tealish: '#00918E',
			primary: '#00b7ff',
			dark: '#0d1117',
			'dark-card': '#161b22',
		  },
		},
	},
	plugins: [],
}
