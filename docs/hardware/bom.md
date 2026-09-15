# Bill of Materials

## 1. Core control and compute

| Qty | Part | Description | Voltage / rating | Approx. cost | Source |
|---|---|---|---|---|---|
| 1 | ESP32-S3-WROOM-1 / N16R8 | ESP32-S3 module with 16MB flash and 8MB PSRAM | 3.3V logic, 5V VIN | $12–18 | Digi-Key / Mouser |
| 1 | RT9080 / AP2112K | 3.3V LDO or buck for ESP32-S3 I/O rail | 3.3V, 1A+ | $1–3 | Digi-Key |
| 1 | LM2596 / MP1584 | 5V buck converter for TFT and auxiliary logic | 5V, 3A | $3–6 | Digi-Key |
| 1 | 12V automotive fuse block | Input protection and service disconnect | 12V, 5–10A | $8–15 | Amazon / Digikey |
| 1 | SMBJ33A / TVS array | Surge protection on 12V input | 33V bidirectional | $1–3 | Digikey |

## 2. Primary outdoor display wall

| Qty | Part | Description | Voltage / rating | Approx. cost | Source |
|---|---|---|---|---|---|
| 18 | 6-inch 7-segment LED display | Common anode or common cathode, high-brightness outdoor rated | 12V | $10–25 each | LED supplier |
| 18 | TPIC6B595 / 74HC595 | Segment/latch shift registers per display row | 5V logic | $1–3 each | TI / Nexperia |
| 6 | AOI518 / IRLZ44N | Row driver MOSFETs for display multiplexing | 12V, 20A | $1–3 each | Digikey |
| 1 | ULN2803A / MOSFET array | Transistor driver for row/blankena signals | 50V | $1–2 | Digikey |
| 3 | 1000uF/16V electrolytic | Bulk stabilization near row drivers | 16V | $0.50–1.50 | Digikey |

## 3. Hoysund touch TFT and touch controller

| Qty | Part | Description | Voltage / rating | Approx. cost | Source |
|---|---|---|---|---|---|
| 1 | 4.0" Hoysund TFT display | Local control panel with touch overlay | 5V | $20–40 | Hoysund / AliExpress |
| 1 | ST7796S or ST7701S panel | SPI TFT controller variant | 3.3V logic | $0–20 | Panel supplier |
| 1 | FT6336U / GT911 | Capacitive touch controller | 3.3V | $2–5 | Digi-Key |
| 1 | 220uF + 0.1uF decoupling | TFT power conditioning | 5V | $0.50–1.50 | Digi-Key |

## 4. Secondary telemetry display

| Qty | Part | Description | Voltage / rating | Approx. cost | Source |
|---|---|---|---|---|---|
| 1 | 12–15" LVDS/eDP panel | Secondary telemetry display | 12V / 5V | $30–80 | LCD supplier |
| 1 | RTD2556 / LVDS to HDMI converter | External display bridge | 5V | $15–30 | AliExpress |
| 1 | 3-pin JST / Molex harness | UART control connection to ESP32 | 3.3V signal | $2–5 | Digi-Key |

## 5. Passive parts and PCB

| Qty | Part | Description | Voltage / rating | Approx. cost | Source |
|---|---|---|---|---|---|
| 1 | 2-layer or 4-layer PCB | Main board with power and display routing | 12V / 5V / 3.3V | $25–80 | PCB fab |
| 1 | 2.54mm terminal block set | Input power and harness breakouts | 12V | $5–12 | Digi-Key |
| 1 | JST-XH/XT30 connectors | 12V, 5V, and logic power distribution | 12V / 5V | $10–20 | Digikey |
| 1 | Heat sink set | Regulator cooling | passive | $5–12 | Digi-Key |

## 6. Thermal and electrical design notes

- The 18-digit display wall is the largest thermal and current load. Expect peak current draw in the tens of amps at 12V if all digits are at full brightness.
- The row driver MOSFETs should be arranged in a perimeter layout with short, wide copper pours to minimize voltage droop and heat rise.
- Keep the ESP32-S3 digital logic and high-current LED return paths physically separated to avoid noise coupling into the SPI and UART traces.
- The board should include test points for 12V input, 5V buck output, 3.3V logic rail, and each display row driver gate signal.
