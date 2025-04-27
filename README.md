# Mini B2EMO Controller

This is code for the Controller which uses an [Adafruit Feather M0 w/RFM95 LoRa](https://www.adafruit.com/product/3178).

The [guide](https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module/overview) has in-depth details on the microcontroller.

Build with Arduino framework, which depends on [RadioHead](https://www.airspayce.com/mikem/arduino/RadioHead/index.html) libraries.

3D Printed files uploaded to [MrBraddeley](https://www.patreon.com/mrbaddeley)

## PlatformIO Setup

The Adafruit board config for PlatforIO is here:
- https://docs.platformio.org/en/latest/boards/atmelsam/adafruit_feather_m0.html

On a MacBook Pro M1, we had to explicitly tell it which COM Port to use:
- https://docs.platformio.org/en/latest/projectconf/sections/env/options/upload/upload_port.html