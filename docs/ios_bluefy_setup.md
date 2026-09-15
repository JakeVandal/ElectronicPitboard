# iOS Bluefy Setup for the Pitboard

This document explains how to run the pitboard interface on iPhone or iPad using Bluefy Web BLE.

## 1. Requirements

- iPhone or iPad running iOS 15+
- Bluefy app installed from the Apple App Store
- A secure URL for the local pitboard service
- ESP32 pitboard powered and advertising with the BLE service name `ESP32-Pitboard`

## 2. Secure hosting options

### Option A: local HTTPS from the project

From the repository root:

```bash
cd /home/jake-vandal/Documents/ElectronicPitboard
python3 -m http.server 8443 --directory webapp
```

If you need a certificate for browser security, use `mkcert`:

```bash
mkcert -install
mkcert localhost 127.0.0.1 ::1
python3 -m http.server 8443 --directory webapp --bind 127.0.0.1
```

Then open `https://localhost:8443` in Bluefy.

### Option B: tunnel for instant phone access

```bash
cloudflared tunnel --url http://localhost:8000
```

Use the generated URL inside Bluefy on the iPhone, then use the Web BLE connection button.

## 3. Bluefy pairing flow

1. Install Bluefy from the App Store.
2. Open the app and switch to the browser tab.
3. Navigate to the secure URL from your local or tunneled endpoint.
4. Tap the pitboard connection button.
5. Allow Bluetooth permission when the native prompt appears.
6. Select the device named `ESP32-Pitboard`.
7. Confirm the GATT service and characteristic access requests.

## 4. Troubleshooting

### No device shows up

- Ensure the ESP32 is powered and advertising.
- Move closer to the board.
- Restart the Bluefy browser tab.
- Confirm there is no iOS background blocker suppressing Web BLE.

### MTU or write failures

- Reduce the payload size.
- Confirm the characteristic UUID matches the board service.
- Retry after reconnecting.

### iOS tab sleep / background issues

- Keep Bluefy in the foreground while pairing.
- Disable heavy background app restrictions while using the board.
- Reopen the page if the device disconnects after a long idle period.

## 5. Security note

Web Bluetooth is only available in secure contexts. Use HTTPS or `localhost` when testing from iPhone or iPad.
