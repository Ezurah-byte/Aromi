<div align="center">
  <!-- You are encouraged to replace this logo with your own! Otherwise you can also remove it. -->
  <img src="logo.png" alt="logo" width="340"  height="auto" />
  <br/>
</div>

# Smart Aromi Tray Embedded Firmware
Welcome to the Smart Aromi Tray project! This document provides an overview of the embedded firmware used to control and manage the Smart Aromi Tray prototype. This firmware integrates various hardware components and ensures seamless communication between the device, mobile app, and cloud services.

## Overview

The Smart Aromi Tray is designed to enhance spice management with an innovative solution combining e-ink displays, NFC technology, load cells, and LED light strips. The firmware operates on an ESP32 microcontroller using the ESP-IDF framework and FreeRTOS, handling data acquisition, processing, and communication.

## Features

- **Data Acquisition and Processing:** Measures spice quantities using load cells, calculates weights with tare adjustments, and displays real-time information on the e-ink screen.
- **User Interface Integration:** Displays spice levels on a high-resolution e-ink screen and uses LED light strips for visual status indicators.
- **Connectivity and Provisioning:**
  - **ESP-NOW Communication:** Facilitates direct communication between ESP32 devices for fast and efficient data exchange, including displaying information on the e-ink screen.
  - **BLE Provisioning:** Enables easy setup by using Bluetooth Low Energy (BLE) to share Wi-Fi credentials and a custom unique ID.
  - **Wi-Fi Connectivity:** Connects to the network for real-time data synchronization.
- **Data Communication:**
  - **MQTT Protocol:** Publishes hardware-collected data (e.g., spice levels) to a central server for cloud-based monitoring and analysis.
- **NFC Integration:** Uses an NFC reader to determine the location of spices in the tray, ensuring accurate slot identification.
- **Power Management:** Operates on a USB power supply for continuous, battery-free operation.

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |


## Installation and Setup

### Prerequisites

- **Hardware:**
  - ESP32 microcontroller
  - Load cells
  - E-ink display
  - NFC reader
  - LED light strips
  - USB power supply

- **Software:** 

<img src="https://openthread.io/static/images/ot-contrib-espressif.png" alt="logo" width="160"  height="auto" />


<img src="https://lorenzoruscitti.altervista.org/wp-content/uploads/2020/12/FreeRTOS-logo-960x540.png" alt="logo" width="160"  height="auto" />

  - [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
  - [FreeRTOS](https://www.freertos.org/)
  - Libraries for ESP-NOW, MQTT, BLE, NFC, and other components

### Steps

1. **Clone the Repository:**
   ```sh
   git clone https://github.com/your-repo/smart-aromi-tray-firmware.git
   cd smart-aromi-tray-firmware
   ```

2. **Install Dependencies:**
   Install required libraries and tools as specified in the ESP-IDF and FreeRTOS documentation.

3. **Configure Wi-Fi and MQTT:**

![Hii](https://th.bing.com/th?q=MQTT+Cloud+Icon&w=120&h=120&c=1&rs=1&qlt=90&cb=1&dpr=1.5&pid=InlineBlock&mkt=en-IN&cc=IN&setlang=en&adlt=moderate&t=1&mw=247)
   - Open `config.h` and set your Wi-Fi credentials, MQTT server details, and other configuration parameters.

4. **Build and Flash:**
   Build the project and flash it to the board, then run the monitor tool to view serial output:
   ```sh
   idf.py -p PORT flash monitor
   ```
   (To exit the serial monitor, type `Ctrl-]`.)

   See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) for full steps to configure and use ESP-IDF to build projects.

5. **Provisioning:**
![Hii](https://assets-global.website-files.com/62bf044fd84e8c677990c16a/645a98e7b98df27162433e7d_MQ_BestPractices_ProviNamin_Blog_1200x800.jpg)
   - Use the mobile app to connect via BLE and share Wi-Fi credentials and a custom unique ID with the ESP32. 
   - **Provisioning QR Code:** To initiate provisioning, scan the QR code displayed during the setup process or visit the following URL for provisioning:
     ```
     I (1472) app: Scan this QR code from the provisioning application for Provisioning.
     I (1472) QRCODE: Encoding below text with ECC LVL 0 & QR Code Version 10
     I (1482) QRCODE: {"ver":"v1","name":"AROMI_EA69FC","pop":"abcd1234","transport":"ble"}
     ```



## Usage

1. **Power On:** Connect the Smart Aromi Tray to a USB power source.
2. **Provision:** Use the mobile app to configure Wi-Fi settings via BLE.
3. **Monitor:** The e-ink display and LED light strips will provide real-time information about spice levels. The NFC reader will help identify the location of spices in their slots.
4. **Data Sync:** The firmware will publish data to the MQTT server for remote monitoring and cloud integration.

## Troubleshooting

- **No Data Displayed:** Ensure the load cells, e-ink display, and NFC reader are properly connected. Check for any errors in the serial monitor.
- **Connectivity Issues:** Verify Wi-Fi credentials and MQTT server settings in `config.h`. Ensure the ESP32 is within range of your network.
- **BLE Provisioning Problems:** Ensure BLE is enabled on your mobile device and the ESP32 is in pairing mode.
- **NFC Issues:** Ensure the NFC reader is functioning correctly and that the NFC tags are properly positioned.

## License

- Newlib is licensed under the BSD License.
- Xtensa header files are Copyright (C) 2013 Tensilica Inc and are licensed under the MIT License as reproduced in the individual header files
- Original parts of FreeRTOS (components/freertos) are Copyright (C) 2017 Amazon.com, Inc. or its affiliates are licensed under the MIT License, as described in license.txt.
- FatFS library, Copyright (C) 2017 ChaN, is licensed under a BSD-style license .
- Xtensa libhal, Copyright (c) Tensilica Inc and licensed under the MIT license (see below)

## Contact

For questions or support, please contact [gowrisankar.m@katomaran.tech](mailto:your-email@example.com)



---

Thank you for using the Smart Aromi Tray embedded firmware! We hope it helps you manage your spices more effectively and enhances your kitchen experience.

---
