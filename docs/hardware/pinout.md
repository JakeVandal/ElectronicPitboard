# ESP32-S3 GPIO Allocation

## Primary allocation

| Function | GPIO | Notes |
|---|---:|---|
| Segment clock | 4 | Shift register serial clock |
| Segment latch | 5 | Storage register enable |
| Segment data | 6 | Serial data output |
| Display blank | 7 | Global blanking / dimming |
| TFT CS | 10 | Hoysund display chip select |
| TFT DC | 11 | Data/command selection |
| TFT RST | 12 | Reset line |
| Touch IRQ | 13 | Capacitive touch interrupt |
| Secondary LCD TX | 17 | UART transmit |
| Secondary LCD RX | 18 | UART receive |
| Secondary LCD reset | 19 | Panel reset |
| Status LED | 48 | Runtime indicator |
| Power sense | 45 | Supply monitoring |
| USB/UART | 20/21 | Programming and serial logging |
| Boot mode | 0 | Must be pulled high for normal boot |

## Notes

- Reserve SPI HS bus lines for high-speed display drivers if using additional peripherals.
- Use a stable ground reference and keep power traces short near high-current LED loads.
- Confirm the final board revision with the actual display module data sheets before manufacturing.
