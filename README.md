# Custom Arduino Keyboard

An embedded systems project implementing a custom keyboard using an Arduino Micro and the NicoHood HID library.  
The project integrates multiple hardware interfaces and peripherals to create a fully functional USB HID keyboard.

## Overview
- Microcontroller: Arduino Micro (ATmega32u4)
- Communication Protocols: I2C, SPI, USB HID
- Peripherals: OLED display, rotary encoder, GPIO expander
- Development Period: Late 2020 – Early 2021

> ⚠️ Note: The original source code for this project has unfortunately since been lost.  
> This repository serves as a technical overview and demonstration of the system design and functionality.

## System Design & Functionality

### Keyboard Matrix
- Implemented a row/column keyboard matrix using GPIO input and output pins
- Firmware continuously scanned outputs and detected key presses via corresponding inputs
- Added diodes per switch to prevent ghosting and unintended key activations

### USB HID
- Used the NicoHood HID library to emulate a USB keyboard
- Translated matrix key presses into standard HID keycodes recognized by the host system

### OLED Display
- Integrated a small OLED display over I2C
- Used the Arduino GFX library to render system status information on-screen

### Rotary Encoder (Volume & Brightness Control)
- Implemented a rotary encoder with an integrated push-button
- Encoder rotation controlled system volume or keyboard backlight brightness
- Push-button toggled between volume and brightness modes

### Backlight Control
- Used PWM to control LED backlight brightness
- Adjusted brightness dynamically based on rotary encoder input

### GPIO Expansion
- Added an MCP23S17-E/SP SPI GPIO expander to overcome microcontroller pin limitations
- Expanded available inputs and outputs for keyboard matrix and peripherals

## Demo
A video demonstrating the working breadboard prototype is available here:  
▶️ https://www.youtube.com/watch?v=LvKr0A5dFDw
