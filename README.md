<div align="center">
  <!-- You are encouraged to replace this logo with your own! Otherwise you can also remove it. -->
  <img src="logo.png" alt="logo" width="340"  height="auto" />
  <br/>
</div>

## App OverView

The **Aromi Smart Tray** is a groundbreaking kitchen solution that incorporates advanced technologies such as **NFC technology** and **Gemini AI**. It help to efficiently manage kitchen spices and condiments, generating intriguing recipes based on the available ingredients. Our product is designed to revolutionize kitchen organization and enhance user convenience through smart technology.

<h4 align="center">This documentation includes instructions for setting up the app as well as the app documentation.</h4>

# App Setup

> **Note**: Make sure you have completed the [React Native - Environment Setup](https://reactnative.dev/docs/environment-setup) instructions


**Clone the Repository:**
   ```sh
   git clone https://gitlab.com/aromi-tray/mobile-app.git
   cd smart-aromi-tray-firmware
   ```

## Step 1: Start the Metro Server

First, you will need to start **Metro**, the JavaScript _bundler_ that ships _with_ React Native.

To start Metro, run the following command from the _root_ of your React Native project:

```bash
# using npm
npm install
# using npm
npm start

# OR using Yarn
yarn start
```

## Step 2: Start your Application

Let Metro Bundler run in its _own_ terminal. Open a _new_ terminal from the _root_ of your React Native project. Run the following command to start your _Android_ or _iOS_ app:

### For Android

```bash
# using npm
npm run android


```

This is one way to run your app — you can also run it directly from within Android Studio and Xcode respectively.

## App File Structure


```
├── __tests__
├── __bundle__
├── .vscode
├── android
├── app
│   ├── animation
│   │   └── starterAnimation.ts
│   ├── assets
│   │   ├── fonts
│   │   ├── images
│   │   ├── lotties
│   │   └── vectors
│   ├── components
│   │   ├── atoms
│   │   ├── molecules
│   │   ├── organisms
│   │   └── example
│   └── hooks
├── library
│   └── features
├── navigation
├── packageModule
├── screens
│   ├── private
│   └── public
├── theme
├── types
└── utils
├── ios
├── node_modules
├── .eslintrc.js
├── .gitignore
├── .prettierrc.js
├── .watchmanconfig
├── app.json
├── App.tsx
├── babel.config.js
├── exttext.tsx
├── folders.png
├── Gemfile
├── index.js
├── intro.svg
├── jest.config.js
├── logo.png
├── metro.config.js
├── nativewind-env.d.ts
├── package-lock.json
├── package.json
├── react-native.config.js
├── README.md
├── sheetTest.tsx
├── tailwind.config.js
└── tsconfig.json
```


## Project Structure
# Root Level

    __tests__/: Contains unit and integration tests for the application.
    __bundle__/: Stores bundled files generated during the build process.
    .vscode/: Configuration files for Visual Studio Code, such as workspace settings.
    android/: Android-specific project files and configurations.
    ios/: iOS-specific project files and configurations.
    node_modules/: Contains all dependencies installed via npm or yarn.

# Application-Specific Directories

    app/: Core directory for application code.
        animation/: Handles animations used in the app.
            starterAnimation.ts: Manages animation logic.
        assets/: Static assets like fonts, images, and vectors.
            fonts/: Font files.
            images/: Static images.
            lotties/: Lottie animations.
            vectors/: Vector images.
        components/: Reusable UI components, structured using atomic design.
            atoms/: Basic UI elements like buttons.
            molecules/: Small groups of UI elements.
            organisms/: Complex UI components.
            example/: Example components or usage examples.
        hooks/: Custom React hooks for encapsulating logic.
        library/: Core features or utility modules.
        features/: Specific features or modules of the application.
        navigation/: Manages the navigation structure of the app.
        packageModule/: Custom modules or packages used within the app.
        screens/: Different screens or pages of the application.
            private/: Screens accessible after authentication.
            public/: Screens accessible without authentication.
        theme/: Theming information like colors and styles used across the app.
        types/: TypeScript type definitions and interfaces.
        utils/: Utility functions reused across the app.

# Configuration and Setup Files

    .eslintrc.js: Configuration for ESLint to identify and fix JavaScript/TypeScript issues.
    .gitignore: Files and directories to be ignored by Git.
    .prettierrc.js: Configuration for Prettier to enforce consistent code formatting.
    .watchmanconfig: Configuration for Watchman, used for watching file changes.
    app.json: App-specific settings like name, display name, and icons.
    App.tsx: Main entry point for the React Native application.
    babel.config.js: Configuration for Babel to compile JavaScript.
    exttext.tsx: Custom file related to extended text components.
    folders.png: A static image, possibly used in documentation or app.
    Gemfile: Ruby dependencies, often used for managing CocoaPods.
    index.js: Entry point for the app's JavaScript bundle.
    intro.svg: SVG file for an introduction screen or component.
    jest.config.js: Configuration for Jest, a testing framework.
    logo.png: The app's logo image.
    metro.config.js: Configuration for Metro bundler.
    nativewind-env.d.ts: TypeScript declaration for NativeWind.
    package-lock.json: Locks dependency versions for consistent installs.
    package.json: Contains project metadata, scripts, and dependencies.
    react-native.config.js: Configuration for React Native CLI.
    README.md: This documentation file.
    sheetTest.tsx: File for testing sheet components.
    tailwind.config.js: Configuration for Tailwind CSS.
    tsconfig.json: TypeScript configuration file.

#  App Features
* **Wi-Fi Provisioning**: Allows the tray to connect to a Wi-Fi network.
* **Spice Management:** Manage spice names and expiry dates.
* **Real-Time Data:** Displays data from the tray.
* **Recipe Generation:** Uses Gemini AI to generate recipes based on spices.

#  Back End
* **Framework:** Flask.
* **Responsibilities** -
  - Communicates with the Gemini API.
  - Recipe generation.

#  ESP Device
* **Hardware:** Equipped with an ESP32 microcontroller and four spice jars.
* **Functionality** -
  - Wi-Fi provisioning.
  - Spice management and data sharing via MQTT



## Technical Architecture
 ## IoT Device Integration

    ESP32 Configuration:
        Wi-Fi Provisioning: The app will list available trays, request Wi-Fi credentials, and facilitate the tray’s connection to the network.
        MQTT Communication: Sends spice data to the server and receives real-time updates.

## App Workflow

    Initial Setup:
        User connects to the tray via the app.
        App requests Wi-Fi credentials (SSID and password).
        Tray connects to Wi-Fi and begins data transmission via MQTT.

    Data Management:
        The app displays real-time spice management data from the tray.
        Users can modify spice names and expiry dates.

    Recipe Generation:
        The app sends spice data to the backend Flask API.
        The Flask API forwards the data to the Gemini API.
        Recipes are retrieved and displayed to the user.

# Troubleshooting




If you can't get this to work, see the [Troubleshooting](https://reactnative.dev/docs/troubleshooting) page.
