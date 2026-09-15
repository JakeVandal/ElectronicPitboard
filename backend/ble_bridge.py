from __future__ import annotations

import asyncio
from typing import Any

from bleak import BleakClient, BleakScanner

SERVICE_UUID = "0000FFB0-0000-1000-8000-00805F9B34FB"
ROW1_UUID = "0000FFB1-0000-1000-8000-00805F9B34FB"
ROW2_UUID = "0000FFB2-0000-1000-8000-00805F9B34FB"
ROW3_UUID = "0000FFB3-0000-1000-8000-00805F9B34FB"


def build_display_rows(rider: dict[str, Any]) -> tuple[str, str, str]:
    position = str(rider.get("position", "P--")).strip()
    last_lap = str(rider.get("last_lap") or "--:--.---").strip()
    best_lap = str(rider.get("best_lap") or "--:--.---").strip()
    speed = str(rider.get("speed_trap") or "0.0 mph").strip()
    bike = str(rider.get("bike_number") or "0").strip()

    row1 = last_lap[:18]
    row2 = f"P {bike:>2} {position[:6]}"[:18]
    row3 = speed[:18]
    return row1, row2, row3


async def discover_pitboard() -> str | None:
    devices = await BleakScanner.discover(timeout=3.0)
    for device in devices:
        if device.name and "ESP32" in device.name:
            return device.address
    return None


async def connect_and_push(rider: dict[str, Any]) -> None:
    address = await discover_pitboard()
    if not address:
        print("No ESP32 pitboard discovered")
        return

    row1, row2, row3 = build_display_rows(rider)
    print(f"BLE address: {address}")
    print(f"Row1={row1!r}")
    print(f"Row2={row2!r}")
    print(f"Row3={row3!r}")

    async with BleakClient(address) as client:
        await client.start_notify(ROW1_UUID, lambda *_: None)
        await client.start_notify(ROW2_UUID, lambda *_: None)
        await client.start_notify(ROW3_UUID, lambda *_: None)
        for uuid, value in [(ROW1_UUID, row1), (ROW2_UUID, row2), (ROW3_UUID, row3)]:
            await client.write_gatt_char(uuid, value.encode("utf-8"), response=True)
        await asyncio.sleep(0.5)


def main() -> None:
    rider = {
        "position": "P2",
        "bike_number": 21,
        "last_lap": "1:25.982",
        "best_lap": "1:25.120",
        "speed_trap": "171.3 mph",
    }
    asyncio.run(connect_and_push(rider))


if __name__ == "__main__":
    main()
