# System Architecture

## 1. Overview

The Electronic Pitboard is a multi-display embedded system built around an ESP32-S3. It combines real-time race telemetry acquisition, human override via BLE, and a deterministic outdoor display pipeline for the pit lane. The architecture is intentionally split into independent tasks so that live timing, local UI rendering, and display refresh all remain responsive even during network instability.

## 2. FreeRTOS task breakdown

- `TaskSegments` (Core 1, Priority 3): refreshes the 18-digit display at a steady interval and writes the current row buffer to the hardware shift register chain.
- `TaskTFT_UI` (Core 1, Priority 2): manages the local Hoysund TFT, LVGL tick update, touch events, and screen transitions.
- `TaskNetwork` (Core 0, Priority 2): connects to Wi-Fi, polls the MotoAmerica timing feed, parses streaming data, and pushes structured telemetry to the display queue.
- `TaskBLE` (Core 0, Priority 1): manages the BLE GATT service, receives manual pit commands, and updates status notifications.

## 3. Inter-task queue structure

- `g_segmentQueue`: display commands for the outdoor panel
- `g_tftQueue`: UI state updates and touch-driven rendering tasks
- `g_networkQueue`: parsed telemetry frames from the streaming scraper
- `g_bleQueue`: override and mode-change events from the BLE control channel
- `g_displayMutex`: protects the active display buffer when multiple tasks post updates

This ensures that the segment display, local TFT, and telemetry pipelines do not corrupt each other during race conditions or rapid mode transitions.

## 4. Runtime state machine

The firmware supports four main operating states:

- `Manual`: the pit crew has taken control through BLE, and the board is driven by operator-entered values.
- `LiveTiming`: the MotoAmerica live feed is being parsed and the display reflects real race timing.
- `Standby`: the display is idle, waiting for new data or a connection.
- `Fault`: the board is in a degraded mode because the network stream is invalid or a hardware fault is present.

The state is kept in the shared `PitStatus` structure and mirrored to the TFT and BLE status characteristics.

## 5. Data flow

1. The ESP32-S3 boots and initializes all GPIO, UART, SPI, and BLE resources.
2. `TaskNetwork` attempts Wi-Fi connection and HTTP polling to `timing_frame.php`.
3. The streaming parser scans chunks for `#server-load`, `#invalid-status`, and rider row blocks.
4. Structured telemetry is dispatched to the display queues once a valid rider row is identified.
5. `TaskSegments` translates the active telemetry into ASCII / 7-segment output for rows 1–3.
6. `TaskTFT_UI` shows tire-pressure or mode state, QR login code, and local status overlays.
7. `TaskBLE` accepts human-written overrides and may override live feed mode when manual control is selected.

## 6. Failure handling

- Lost Wi-Fi triggers reconnect attempts without blocking the display task.
- Partial or malformed HTTP payloads are discarded instead of corrupting the current board buffer.
- Manual override takes precedence over live timing once BLE mode is selected.
- When telemetry is stale or invalid, the board falls back to the last known-good values and a bright warning message.
