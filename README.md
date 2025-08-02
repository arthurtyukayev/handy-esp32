# Handy.computer ESP32

A ESP32-based project for M5Stack devices that provides Bluetooth connectivity, battery management, keyboard input, LED control, and screen display functionality. Specifically built-in to be used with [handy.computer](https://handy.computer).

## Supported Boards

This project supports the following M5Stack devices:

1. [**M5StickC Plus 2**](https://shop.m5stack.com/products/m5stickc-plus2-esp32-mini-iot-development-kit)
   - Features: Built-in display, battery management, Bluetooth
   - Main file: `src/main-stick.cpp`

2. [**M5Stack ATOM Lite**](https://shop.m5stack.com/products/atom-lite-esp32-development-kit)
   - Features: NeoPixel LED, Bluetooth connectivity, battery management
   - Main file: `src/main-atom.cpp`

## Prerequisites

- [PlatformIO](https://platformio.org/install)

## Building and Flashing

### For M5StickC Plus 2

1. Build the project:
   ```
   pio run -e m5stack-stickc-plus2
   ```

2. Upload to device:
   ```
   pio run -e m5stack-stickc-plus2 -t upload
   ```

3. Monitor serial output:
   ```
   pio device monitor --port [PORT] --baud 115200
   ```

### For M5Stack Atom

1. Build the project:
   ```
   pio run -e m5stack-atom
   ```

2. Upload to device:
   ```
   pio run -e m5stack-atom -t upload
   ```

3. Monitor serial output:
   ```
   pio device monitor --port [PORT] --baud 115200
   ```

## Project Structure

- `src/` - Source code, most of the code is in here.
  - `assets/` - Image assets
  - `battery/` - Battery management
  - `bluetooth/` - Bluetooth functionality
  - `keyboard/` - Keyboard input handling
  - `led/` - LED control
  - `screen/` - Screen display functionality
  - `main-atom.cpp` - Entry point for Atom
  - `main-stick.cpp` - Entry point for StickC Plus 2
  - `shared.h` - Shared definitions

## Development Guidelines

- Built on Arduino framework and M5Unified library
- Uses task-based architecture with FreeRTOS for concurrency
