### Temperature Sensor Project
Project created in 2024, uploaded 2025

## Goal
Use a Raspberry Pi Pico 2 coded with the Arduino IDE to measure and compare temperatures and enable and disable heating or cooling devices as needed

## Materials
- Raspberry Pi Pico 2
- DS18B20 Sensors
- Relays
- LCD (2x16 Screen)
- Fan (cooling device)
- Strong LED Light Strip (heating device)
- External power banks (5V and 12V battery)

## Wiring Guide
![Image](https://github.com/user-attachments/assets/b57154a3-a041-45cf-98d6-613434e9e5ea)

## Run Guide
Make sure the Arduino IDE includes earlephilhowe's Raspberry Pi board controller in File -> Preferences -> Additional Board Manager URL. Follow the wiring guide and upload the TemperatureSensorComplete.ino file into the Rasperberry Pi Pico 2.

## Libraries Used
- Wire
- OneWire
- DallasTemperature
- LiquidCrystal I2C
