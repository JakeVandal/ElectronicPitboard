# ESP32-S3 GPIO Allocation

## 1. Primary board allocation

| Function | GPIO | Notes |
|---|---:|---|
| `SEG_DATA` | 11 | SPI MOSI for segment display shift chain |
| `SEG_CLK` | 12 | Shift register clock |
| `SEG_LATCH` | 10 | Latch / CS strobe for display buffer transfer |
| `SEG_OE_PWM` | 9 | Output enable and brightness PWM |
| `TFT_MOSI` | 13 | Hoysund TFT SPI MOSI |
| `TFT_SCLK` | 14 | Hoysund TFT SPI SCK |
| `TFT_CS` | 15 | Hoysund TFT chip select |
| `TFT_DC` | 2 | TFT data/command signal |
| `TFT_RST` | 4 | TFT reset |
| `TFT_BL` | 5 | Backlight PWM |
| `TOUCH_SDA` | 17 | I2C touch data |
| `TOUCH_SCL` | 18 | I2C touch clock |
| `TOUCH_INT` | 16 | Touch interrupt |
| `TOUCH_RST` | 21 | Touch reset |
| `TELEM_TX` | 43 | UART2 transmit to telemetry display |
| `TELEM_RX` | 44 | UART2 receive |
| `STATUS_LED` | 48 | Runtime status indicator |
| `POWER_SENSE` | 45 | Battery / rail sense input |
| `USB_TX` | 20 | UART programmer output |
| `USB_RX` | 21 | UART programmer input |
| `BOOT` | 0 | Must remain high during normal boot |

## 2. Reserved and multipurpose pins

| Pin | Role |
|---|---|
| 1, 3, 6, 7, 8, 19, 22–41, 46–47 | Available for future expansion / alternative sensor routing |
| 45 | Power sense input; avoid switching heavy loads on this rail |
| 48 | LED indicator; keep short to avoid EMI |

## 3. Interface notes

- SPI2 / FSPI is recommended for the segment driver and TFT bus to keep timing deterministic.
- UART2 is reserved for the secondary telemetry display at 921600 baud.
- Keep the TFT and segment driver clocks away from the Wi-Fi antenna ground region.
- The boot pin must not be pulled low unless forcing flash mode during programming.
