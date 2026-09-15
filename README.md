# ESP32-S3 Smart Pitboard

A production-oriented electronic racing pitboard and telemetry system built around an ESP32-S3. The platform combines live MotoAmerica timing feed parsing, a manual pit-crew control interface over Web Bluetooth, and multiple display outputs for the paddock and rider-facing surfaces.

## Features

- 18-digit, 6-inch 7-segment pit wall display driven by SPI + shift register topology
- 4.0" Hoysund touch TFT for local control UI and QR code / status telemetry
- 12–15" external telemetry display driven by a UART/SPI pipeline
- Non-blocking Wi-Fi connectivity and MotoAmerica timing feed parsing
- Manual override and status updates over BLE from a phone or browser web app
- Outdoor-readable high-contrast UI and responsive pit-crew control interface

## Repository layout

- `firmware/` – PlatformIO firmware and hardware abstractions
- `webapp/` – PWA for manual pit control over Web Bluetooth
- `docs/` – hardware, architecture, and implementation documentation
- `.github/workflows/` – CI pipelines for firmware and web app validation

## Hardware overview

This repository is structured around an ESP32-S3 WROOM-1 / N16R8 module with 16MB flash and 8MB PSRAM. The design uses:

- 3-row segment display driver chain for the 18-digit wall
- TFT touch display for local UI and manual pit commands
- External telemetry screen for live lap/position data
- BLE control channel for browser-based operator input
- Wi-Fi station mode for secure connectivity to the race timing feed

## Quick start

### Firmware

```bash
cd firmware
pio run -t upload
pio test
```

### Web app

Open the app in a browser or host it using a static file server:

```bash
cd webapp
python3 -m http.server 8080
```

Then open: `http://localhost:8080`

## BLE and web app workflow

- The ESP32 exposes a GATT service for pit commands and operational status.
- The browser web app connects over Web Bluetooth to send manual timing messages.
- If the connection is unavailable or the race feed is stale, firmware falls back to local manual mode.

## Documentation

- [docs/architecture.md](docs/architecture.md)
- [docs/hardware/bom.md](docs/hardware/bom.md)
- [docs/hardware/schematics_guide.md](docs/hardware/schematics_guide.md)
- [docs/hardware/pinout.md](docs/hardware/pinout.md)

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE).
