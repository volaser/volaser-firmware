# Volaser Firmware
Currently the firmware for the volaser is for two separate microcontrollers, one located in the laser head which handles area and depth measurement, and another located outside the laser head which handles raising and lowering the measurement device into the septic tank. It is designed to use the [SparkFun ESP32 Thing](https://www.sparkfun.com/products/13907) microcontroller and the Volaser PCB, but any ESP32 based microcontroller should work, with appropriate modifications.

# Arduino IDE Setup

The easiest way to compile and install the firmware is to use the Arduino IDE. This can be downloaded from https://www.arduino.cc/en/Main/Software. 

Install support for ESP32 boards using the Arduino IDE Boards Manager

- Install the current upstream Arduino IDE at the 1.8 level or later. The current version is at the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Preferences window.
- Enter ```https://dl.espressif.com/dl/package_esp32_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp32* platform (and don't forget to select your ESP32 board from Tools > Board menu after installation).

# Installation
- Select the "SparkFun ESP32 Thing" from Tools > Board (or a different board, if you are using a different ESP32 microcontroller).
- Compile the sketch with Sketch > Verify/Compile. If everything is correct it should compile at this point without any errors.
- Plug in the SparkFun ESP32 microcontroller to your computer using a microusb cable. Select the appropriate port from Tools > Port. Upload the sketch to the microcontroller with Sketch > Upload.
- The device should now be detectable by the volaser app via bluetooth.