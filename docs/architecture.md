# System Architecture

## Overview

The Smart Pitboard is organized as a layered embedded system with a deterministic display pipeline, a network ingestion layer, and a BLE/web control plane. The design is built to be robust under racing conditions: reliable UI rendering, low-latency display updates, and graceful fallback when telemetry or BLE connectivity is lost.

## Data flow

1. The ESP32-S3 establishes Wi-Fi station mode and connects to the local race timing network.
2. A lightweight HTTP client downloads timing frames or raw page fragments from the MotoAmerica timing endpoint.
3. A parser extracts rider position, lap count, delta, and speed values.
4. Parsed payloads are passed into a queue-based state update pipeline.
5. The display manager updates the 7-segment wall, Hoysund TFT, and external telemetry LCD.
6. The BLE service exposes manual override commands and current status to the pit crew web app.

## State machine

The firmware runs a bounded state machine with three primary modes:

- Manual: operator-controlled message or screen state, safe for pit-wall commands.
- Live timing: race feed is active and the system is showing live telemetry.
- Fault: degraded or invalid state where the system uses a fallback message and status indicators.

## Parser mechanics

The MotoAmerica data source is treated as an untrusted streaming endpoint. The parser should:

- handle chunked or partial HTTP responses safely,
- scan for known timing markers rather than assuming a perfectly structured document,
- enforce buffer bounds and reject malformed payloads,
- preserve the last-known-good telemetry to avoid flashing invalid values.

## Display ownership

The system splits responsibilities across multiple display owners:

- Segment driver: low-level 7-segment rendering and multiplexing
- Hoysund TFT: local UI, touch input, QR code, and on-board diagnostics
- External telemetry display: high-contrast rider-facing data grid

## Failure handling

- Wi-Fi loss triggers retry logic without blocking the render loop.
- Invalid timing frames are discarded and the last stable values remain visible.
- BLE manual mode automatically overrides live data when operator input is active.
- Displays gracefully degrade to a standby banner if power or communications are unavailable.
