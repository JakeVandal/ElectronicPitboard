# Bill of Materials

## Core compute

- ESP32-S3-WROOM-1 or N16R8 module with 16MB flash and 8MB PSRAM
- 3.3V regulator rail with low-noise buck converter
- Polyfuse / resettable fuse for 12V input protection
- TVS diode protection and reverse-polarity protection
a

## Display hardware

- 18-digit 6-inch 7-segment display wall (3 rows x 6 digits)
- 74HC595 or TPIC6B595 shift register chain for digit and segment control
- MOSFET or LED driver array for row multiplexing and brightness control
- 4.0" Hoysund touch TFT controller with capacitive touch input
- 12–15" secondary telemetry LCD panel with UART or SPI interface
- Optional transistor array for local backlight and panel power switching

## Power distribution

- 12V primary input from race support or vehicle battery
- 5V buck converter for digital logic and TFT supply
- 3.3V regulator for ESP32-S3 and logic rails
- Per-panel current measurement and low-voltage cutoff where required

## Interfaces

- SPI bus for segment driver and TFT controller
- UART for LCD telemetry pipeline
- I2C or GPIO-based touch controller lines
- BLE and Wi-Fi antenna paths with proper ground plane clearance

## Miscellaneous

- Fuse blocks, terminal strips, and weather-resistant connectors
- Heatsink or passive cooling for power regulators
- Enclosure with ventilation and outdoor-readable contrast considerations
