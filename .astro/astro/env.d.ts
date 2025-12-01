declare module 'astro:env/client' {
	export const PUBLIC_SERVER: string;	
export const REPUBLIC_SERVER: string;	

}

declare module 'astro:env/server' {
	

	export const getSecret: (key: string) => string | undefined;
}
