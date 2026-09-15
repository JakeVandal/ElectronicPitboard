#!/usr/bin/env python3
"""Minimal BLE pit-control client for the ESP32 Smart Pitboard."""

from __future__ import annotations

import argparse
import asyncio
import json
import sys
from typing import Any

from bleak import BleakClient, BleakScanner
from rich.console import Console
from rich.live import Live
from rich.table import Table

SERVICE_UUID = "0000FFB0-0000-1000-8000-00805F9B34FB"
ROW1_UUID = "0000FFB1-0000-1000-8000-00805F9B34FB"
ROW2_UUID = "0000FFB2-0000-1000-8000-00805F9B34FB"
ROW3_UUID = "0000FFB3-0000-1000-8000-00805F9B34FB"
CONTROL_UUID = "0000FFB4-0000-1000-8000-00805F9B34FB"
STATUS_UUID = "0000FFB5-0000-1000-8000-00805F9B34FB"

console = Console()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Pit wall BLE control client")
    parser.add_argument("--scan", action="store_true", help="List nearby ESP32-Pitboard devices")
    parser.add_argument("--name", default="ESP32-Pitboard", help="BLE device name to connect to")
    parser.add_argument("--mock", action="store_true", help="Use mock mode without a live BLE connection")
    return parser.parse_args()


async def discover_devices() -> list[str]:
    devices = await BleakScanner.discover(timeout=3.0)
    names = []
    for device in devices:
        if device.name:
            names.append(f"{device.name} ({device.address})")
    return names


async def main() -> int:
    args = parse_args()

    if args.scan:
        devices = await discover_devices()
        if not devices:
            console.print("No BLE devices found.")
            return 0
        console.print("Discovered devices:")
        for item in devices:
            console.print(f" - {item}")
        return 0

    if args.mock:
        console.print("[yellow]Mock mode active: no BLE device connection is attempted.[/yellow]")
        console.print("Use keys: B=BOX, P=PUSH, 1-9=position, +/-=gap adjust")
        while True:
            key = console.input("pit> ").strip()
            if not key:
                continue
            console.print(f"Mock command: {key}")
        return 0

    table = Table(title="Pitboard status")
    table.add_column("Field")
    table.add_column("Value")
    table.add_row("Device", args.name)
    table.add_row("Service", SERVICE_UUID)
    table.add_row("Mode", "scan")

    with Live(table, refresh_per_second=4) as live:
        async with BleakClient(args.name) as client:
            try:
                services = await client.get_services()
                service = services.get_service(SERVICE_UUID)
                if service:
                    live.update(table)
                table.add_row("Status", "Connected")
                live.update(table)
            except Exception as exc:  # pragma: no cover - CLI UX fallback
                table.add_row("Status", f"Error: {exc}")
                live.update(table)
                await asyncio.sleep(1.5)

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(asyncio.run(main()))
    except KeyboardInterrupt:
        console.print("\nInterrupted.")
        raise SystemExit(130)
