import { defineConfig } from 'vite';
import preact from '@preact/preset-vite';

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [preact()],
  build: {
    rollupOptions: {
      output: {
        entryFileNames: 'assets/bundle.js', 
        chunkFileNames: 'assets/chunk-[name].js', 
        assetFileNames: ({ name }) => {
          // Determine the correct extension based on the asset's content type
          if (/\.(gif|jpe?g|png|svg)$/.test(name ?? '')) {
            return 'assets/[name].[ext]';
          } 
          if (/\.css$/.test(name ?? '')) {
            return 'css/[name].[ext]';
          }
		  if (/\.js$/.test(name ?? '')) {
            return 'js/[name].[ext]';
          }
          // Add more specific cases if needed

          // Fallback for other asset types
          return 'assets/[name].[ext]'; 
        }
      }
    }
  },
});