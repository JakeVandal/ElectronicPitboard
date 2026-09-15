# Electronic Pitboard Runbook

The timing source is `http://timing.motoamerica.com/timing_frame.php`. The Python backend normalizes only position, rider number/name, last lap, best lap, dynamic S1-S5 sectors, and speed trap.

## Linux (bash)

```bash
cd /home/jake-vandal/Documents/ElectronicPitboard
python3 -m venv .venv
. .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
python -m pytest backend/test_scraper.py -q
```

Start the local API and PWA:

```bash
python run.py --mode server --host 0.0.0.0 --port 8000
```

Generate a LAN certificate and start HTTPS:

```bash
python -m backend.generate_cert --output backend/certs --host localhost --host 127.0.0.1
python backend/server.py --host 0.0.0.0 --port 8443 --ssl-cert backend/certs/cert.pem --ssl-key backend/certs/key.pem
```

Start the native PC app in another terminal:

```bash
. .venv/bin/activate
python -m apps.pc_pitboard_app --api http://127.0.0.1:8000
```

Or launch the API and PC app together:

```bash
python run.py --mode all --port 8000
```

Flash and monitor firmware:

```bash
cd firmware
pio run --target upload
pio device monitor -b 115200
```

## macOS (zsh)

```zsh
cd /home/jake-vandal/Documents/ElectronicPitboard
python3 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
python -m pytest backend/test_scraper.py -q
python run.py --mode server --host 0.0.0.0 --port 8000
```

In a second terminal:

```zsh
cd /home/jake-vandal/Documents/ElectronicPitboard
source .venv/bin/activate
python -m apps.pc_pitboard_app
```

## Windows PowerShell

```powershell
Set-Location $HOME\Documents\ElectronicPitboard
py -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
python -m pytest backend/test_scraper.py -q
python run.py --mode server --host 0.0.0.0 --port 8000
```

In a second PowerShell window:

```powershell
Set-Location $HOME\Documents\ElectronicPitboard
.\.venv\Scripts\Activate.ps1
python -m apps.pc_pitboard_app
```

## Native BLE operation

Power the board and confirm it advertises its configured BLE service UUID. Click `SCAN + CONNECT` in the desktop app. The app polls `/api/riders`, displays the live roster, writes the selected rows when the corresponding GATT characteristics exist, and sends quick actions to the control characteristic.

On Linux, BlueZ must be running and the user must have Bluetooth permissions. On Windows and macOS, enable Bluetooth in system settings before launching the app.

## Bluefy / iOS

1. Start the HTTPS server on the same Wi-Fi network as the phone.
2. Find the computer LAN address with `hostname -I` on Linux or `ipconfig` on Windows.
3. Open `https://<computer-ip>:8443` in Bluefy.
4. Accept the development certificate warning.
5. Tap `Connect BLE` and select the ESP32 pitboard.
6. Keep Bluefy in the foreground during operation; iOS may suspend background BLE pages.

See [ios_bluefy_setup.md](ios_bluefy_setup.md) for certificate and pairing details.

## Troubleshooting

- `Address already in use`: find the old process with `ss -ltnp | grep 8000` or choose another port.
- `No module named uvicorn`: activate `.venv` or run `python -m pip install -r requirements.txt`.
- No BLE device: power-cycle the board, confirm Bluetooth permissions, and keep the computer close to the board.
- HTTPS certificate mismatch: regenerate with `python -m backend.generate_cert --output backend/certs --host <computer-ip>` and use that IP in Bluefy.
