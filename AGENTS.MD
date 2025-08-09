# PlatformIO ESP32 Project Guidelines

## Build Commands
- Build specific environment: `pio run -e m5stack-stickc-plus2` or `pio run -e m5stack-atom`
- Upload to device: `pio run -e [env-name] -t upload`
- Monitor serial output: `pio device monitor --port [PORT] --baud 115200` 

## Code Style Guidelines
- Use header guards with `#pragma once` in all header files
- Place includes in order: project headers first, then system/library headers
- Use task-based architecture with FreeRTOS tasks for concurrency
- Use Arduino framework and M5Unified library for hardware access
- Naming: snake_case for variables/functions, PascalCase for classes, ALL_CAPS for constants
- Declare task handles and semaphores as external in shared header files
- Use proper error handling with ESP32 log levels (CORE_DEBUG_LEVEL)
- Follow C++ best practices: avoid global variables, use references over pointers
- Avoid using comments, instead use meaningful names and self-documenting code
