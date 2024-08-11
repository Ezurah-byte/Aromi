module.exports = {
  presets: ['module:@react-native/babel-preset'],
  plugins: [
    'react-native-reanimated/plugin',
    [
      'module-resolver',
      {
        root: ['./app'],
        extentions: ['.ios.ts', '.android.ts', '.js', '.ts', '.tsx', '.json'],
        alias: {
          tests: ['./tests/'],
          '@images': './app/assets/images',
          '@lotties': './app/assets/lotties',
          '@vectors': './app/assets/vectors',
          '@theme': './app/theme',
          '@types': './app/types',
          '@library': './app/library',
          '@hooks': './app/hooks',
          '@utils': './app/utils',
          '@atoms': './app/components/atoms/index',
          '@molecules': './app/components/molecules/index',
          '@public': './app/screens/public/index',
          '@private': './app/screens/private/index',
          '@hooks': './app/hooks/index',
          '@root': './app/navigation/rootNavigation',
          '@animation': './app/animation',
        },
      },
    ],
    'nativewind/babel',
  ],
};
