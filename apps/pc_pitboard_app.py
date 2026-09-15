from __future__ import annotations

import asyncio
import threading
from typing import Any, Callable

try:
    import customtkinter as ctk
except ModuleNotFoundError:
    ctk = None  # type: ignore[assignment]
import httpx
from bleak import BleakClient, BleakScanner
from bleak.exc import BleakError

try:
    from apps.ui_components import COLORS, format_preview_rows
except ImportError:  # Direct `python apps/pc_pitboard_app.py` execution.
    from ui_components import COLORS, format_preview_rows

SERVICE_UUID = "0000FFB0-0000-1000-8000-00805F9B34FB"
CHAR_ROW1_TIME = "0000FFB1-0000-1000-8000-00805F9B34FB"
CHAR_ROW2_POS = "0000FFB2-0000-1000-8000-00805F9B34FB"
CHAR_ROW3_GAP = "0000FFB3-0000-1000-8000-00805F9B34FB"
CHAR_CONTROL = "0000FFB4-0000-1000-8000-00805F9B34FB"
API_URL = "http://127.0.0.1:8000"


class NativePitboardApp((ctk.CTk if ctk is not None else object)):
    def __init__(self, api_url: str = API_URL) -> None:
        super().__init__()
        self.api_url = api_url.rstrip("/")
        self.selected_rider: dict[str, Any] | None = None
        self.client: BleakClient | None = None
        self.ble_loop = asyncio.new_event_loop()
        self.ble_thread = threading.Thread(target=self.ble_loop.run_forever, daemon=True)
        self.ble_thread.start()
        self.roster: list[dict[str, Any]] = []
        self.title("Native Pitboard Control")
        self.geometry("1000x720")
        self.configure(fg_color=COLORS["background"])
        self._build_ui()
        self.after(1000, self.refresh_timing)
        self.protocol("WM_DELETE_WINDOW", self.close)

    def _build_ui(self) -> None:
        ctk.set_appearance_mode("dark")
        header = ctk.CTkFrame(self, fg_color=COLORS["panel"])
        header.pack(fill="x", padx=16, pady=16)
        ctk.CTkLabel(header, text="PITBOARD / NATIVE BLE", text_color=COLORS["green"], font=("TkFixedFont", 24, "bold")).pack(side="left", padx=14, pady=14)
        self.status = ctk.CTkLabel(header, text="Disconnected", text_color=COLORS["red"])
        self.status.pack(side="right", padx=14)
        ctk.CTkButton(header, text="SCAN + CONNECT", command=self.connect_ble, fg_color=COLORS["green"], text_color="#001500").pack(side="right", padx=10)

        controls = ctk.CTkFrame(self, fg_color=COLORS["panel"])
        controls.pack(fill="x", padx=16, pady=(0, 12))
        ctk.CTkLabel(controls, text="RIDER").grid(row=0, column=0, padx=10, pady=12)
        self.rider_menu = ctk.CTkComboBox(controls, values=["No roster"], command=self.select_rider)
        self.rider_menu.grid(row=0, column=1, padx=10, pady=12, sticky="ew")
        self.bike_entry = ctk.CTkEntry(controls, placeholder_text="Bike #")
        self.bike_entry.grid(row=0, column=2, padx=10, pady=12)
        ctk.CTkButton(controls, text="APPLY", command=self.apply_manual_rider).grid(row=0, column=3, padx=10)
        controls.grid_columnconfigure(1, weight=1)

        board = ctk.CTkFrame(self, fg_color="#000000", border_color=COLORS["green"], border_width=1)
        board.pack(fill="x", padx=16, pady=12)
        self.row_labels: list[ctk.CTkLabel] = []
        for label in ("ROW 1  LAST LAP", "ROW 2  POSITION", "ROW 3  SPEED"):
            row = ctk.CTkFrame(board, fg_color="#000000")
            row.pack(fill="x", padx=18, pady=8)
            ctk.CTkLabel(row, text=label, width=150, anchor="w", text_color=COLORS["amber"]).pack(side="left")
            value = ctk.CTkLabel(row, text="--:--.--", text_color=COLORS["green"], font=("TkFixedFont", 34, "bold"))
            value.pack(side="left", padx=20)
            self.row_labels.append(value)

        self.telemetry = ctk.CTkLabel(self, text="No rider selected", anchor="w", justify="left")
        self.telemetry.pack(fill="x", padx=22, pady=8)
        self.sectors = ctk.CTkFrame(self, fg_color=COLORS["panel"])
        self.sectors.pack(fill="x", padx=16, pady=8)
        self.sector_labels: list[ctk.CTkLabel] = []

        actions = ctk.CTkFrame(self, fg_color=COLORS["panel"])
        actions.pack(fill="x", padx=16, pady=12)
        for action in ("BOX NOW", "PUSH", "P-IN", "FUEL", "+0.5", "-0.5", "L1", "L2"):
            ctk.CTkButton(actions, text=action, command=lambda item=action: self.send_control(item), width=92).pack(side="left", padx=4, pady=10)
        ctk.CTkLabel(actions, text="Brightness").pack(side="left", padx=(20, 4))
        self.brightness = ctk.CTkSlider(actions, from_=0, to=100, command=self.set_brightness)
        self.brightness.set(100)
        self.brightness.pack(side="left", padx=4)
        ctk.CTkButton(actions, text="BLANK", command=lambda: self.send_control("BLANK")).pack(side="left", padx=8)

    def connect_ble(self) -> None:
        asyncio.run_coroutine_threadsafe(self._connect_ble(), self.ble_loop)

    async def _connect_ble(self) -> None:
        self.set_status("Scanning...", COLORS["amber"])
        device = await BleakScanner.find_device_by_filter(lambda d, ad: SERVICE_UUID.lower() in [str(uuid).lower() for uuid in ad.service_uuids] or "pitboard" in (d.name or "").lower(), timeout=5)
        if device is None:
            self.set_status("Board not found", COLORS["red"])
            return
        try:
            self.client = BleakClient(device, disconnected_callback=lambda _: self.after(0, lambda: self.set_status("Disconnected", COLORS["red"])))
            await self.client.connect()
            self.set_status(f"Connected: {device.name or device.address}", COLORS["green"])
        except Exception as error:
            self.set_status(f"BLE error: {error}", COLORS["red"])

    def send_control(self, text: str) -> None:
        asyncio.run_coroutine_threadsafe(self._write_characteristic(CHAR_CONTROL, text), self.ble_loop)
        self.update_preview((text, self.row_labels[1].cget("text"), self.row_labels[2].cget("text")))

    async def _write_characteristic(self, uuid: str, text: str) -> None:
        if self.client and self.client.is_connected:
            await self.client.write_gatt_char(uuid, text[:18].encode(), response=True)

    def set_brightness(self, value: float) -> None:
        self.send_control(f"BRIGHT:{int(value)}")

    def refresh_timing(self) -> None:
        threading.Thread(target=self._fetch_timing, daemon=True).start()
        self.after(2000, self.refresh_timing)

    def _fetch_timing(self) -> None:
        try:
            response = httpx.get(f"{self.api_url}/api/riders", timeout=3)
            response.raise_for_status()
            roster = response.json()
            self.after(0, lambda: self.update_roster(roster))
        except (httpx.HTTPError, ValueError):
            pass

    def update_roster(self, roster: list[dict[str, Any]]) -> None:
        self.roster = roster
        values = [f"{r.get('position', '--')} #{r.get('bike_number')} - {r.get('rider_name', '')}" for r in roster]
        self.rider_menu.configure(values=values or ["No roster"])
        if roster and self.selected_rider is None:
            self.selected_rider = roster[0]
            self.rider_menu.set(values[0])
            self.render_rider(roster[0])

    def select_rider(self, choice: str) -> None:
        index = self.rider_menu.cget("values").index(choice) if choice in self.rider_menu.cget("values") else -1
        if index >= 0 and index < len(self.roster):
            self.selected_rider = self.roster[index]
            self.render_rider(self.selected_rider)

    def apply_manual_rider(self) -> None:
        try:
            number = int(self.bike_entry.get())
        except ValueError:
            return
        for rider in self.roster:
            if rider.get("bike_number") == number:
                self.selected_rider = rider
                self.render_rider(rider)
                return
        self.telemetry.configure(text=f"Bike #{number} is not currently in the active roster")

    def render_rider(self, rider: dict[str, Any]) -> None:
        rows = format_preview_rows(rider)
        self.update_preview(rows)
        sectors = rider.get("sectors", {})
        for widget in self.sector_labels:
            widget.destroy()
        self.sector_labels = []
        for name, value in sectors.items():
            label = ctk.CTkLabel(self.sectors, text=f"{name}\n{value}", text_color=COLORS["purple"], width=110)
            label.pack(side="left", padx=8, pady=12)
            self.sector_labels.append(label)
        self.telemetry.configure(text=f"#{rider.get('bike_number')} {rider.get('rider_name')}   Last {rider.get('last_lap')}   Best {rider.get('best_lap')}   Speed {rider.get('speed_trap')}")
        for uuid, value in zip((CHAR_ROW1_TIME, CHAR_ROW2_POS, CHAR_ROW3_GAP), rows):
            asyncio.run_coroutine_threadsafe(self._write_characteristic(uuid, value), self.ble_loop)

    def update_preview(self, rows: tuple[str, str, str]) -> None:
        for label, value in zip(self.row_labels, rows):
            label.configure(text=value)

    def set_status(self, text: str, color: str) -> None:
        self.after(0, lambda: self.status.configure(text=text, text_color=color))

    def close(self) -> None:
        if self.client:
            asyncio.run_coroutine_threadsafe(self.client.disconnect(), self.ble_loop)
        self.ble_loop.call_soon_threadsafe(self.ble_loop.stop)
        self.destroy()


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--api", default=API_URL)
    parser.add_argument("--scan-only", action="store_true", help="Scan BLE without opening the GUI")
    args = parser.parse_args()
    if args.scan_only or ctk is None:
        if ctk is None:
            print("CustomTkinter/Tk is unavailable. Install the platform Tk package, then reinstall requirements.")
            print("Linux: sudo apt install python3-tk, then recreate the venv with the matching system Python.")
            print("Running native BLE scan fallback instead.")
        asyncio.run(scan_only())
        return
    app = NativePitboardApp(args.api)
    app.mainloop()


async def scan_only() -> None:
    print("Scanning for ESP32 pitboards using the native OS Bluetooth stack...")
    try:
        devices = await BleakScanner.discover(timeout=5.0)
    except BleakError as error:
        print(f"Bluetooth scan unavailable: {error}")
        print("Enable Bluetooth and confirm the OS adapter is powered on.")
        return
    matches = [device for device in devices if "pitboard" in (device.name or "").lower() or "esp32" in (device.name or "").lower()]
    if not matches:
        print("No ESP32 pitboard found. Confirm power, advertising, and Bluetooth permissions.")
        return
    for device in matches:
        print(f"{device.name or 'Unnamed'}: {device.address}")


if __name__ == "__main__":
    main()
