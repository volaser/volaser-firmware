# Volaser Firmware

The Volaser firmware is written in the Arduino IDE, and uses a microcontroller to make measurements with a TFMini lidar rangefinder, which relays those measurements over a serial connection. It is designed to use the [SparkFun ESP32 Thing](https://www.sparkfun.com/products/13907) microcontroller, but any microcontroller should work, with appropriate modification to the pinouts.

# Arduino IDE Setup

The easiest way to compile and install the firmware is to use the Arduino IDE. This can be downloaded from https://www.arduino.cc/en/Main/Software. 

## Install support for ESP32 boards using the Arduino IDE Boards Manager

- Install the current upstream Arduino IDE at the 1.8 level or later. The current version is at the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Preferences window.
- Enter ```https://dl.espressif.com/dl/package_esp32_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp32* platform (and don't forget to select your ESP32 board from Tools > Board menu after installation).

## Install required libraries

The firmware uses the [SerialCommands](https://github.com/ppedro74/Arduino-SerialCommands) library by Pedro Tiago Pereira
for it's serial interface, and the [TfMini](https://github.com/opensensinglab/tfmini) by Peter Jansen library to communicate with the lidar. These should be installed in the [Arduino IDE library manager](https://www.arduino.cc/en/guide/libraries).

- Select Tools > Manage Libraries (Shift+Ctrl+I).
- Search for "SerialCommands" and click Install
- Search for "TfMini" and click Install. Make sure to install the correct library (it should say by Peter Jansen).

# Installation

- Select the "SparkFun ESP32 Thing" from Tools > Board (or a different board, if you are using a different ESP32 microcontroller).
- Compile the sketch with Sketch > Verify/Compile. If everything is correct it should compile at this point without any errors.
- Plug in the SparkFun ESP32 microcontroller to your computer using a microusb cable. Select the appropriate port from Tools > Port. Upload the sketch to the microcontroller with Sketch > Upload.
- The device should now be detectable by the volaser app via bluetooth.
  