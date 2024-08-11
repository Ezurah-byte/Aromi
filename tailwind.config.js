const colors = require('./app/theme/tailwindColors.js');
const fonts = require('./app/theme/fonts.js');

/** @type {import('tailwindcss').Config} */

module.exports = {
  content: ['./App.{js,jsx,ts,tsx}', './app/**/*.{js,jsx,ts,tsx}'],
  theme: {
    extend: {
      fontFamily: fonts,
      colors: colors,
    },
  },
  plugins: [],
};
