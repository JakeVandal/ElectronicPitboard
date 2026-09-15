from __future__ import annotations

from typing import Any


COLORS = {
    "background": "#050805",
    "panel": "#101610",
    "green": "#62ff7a",
    "amber": "#ffc857",
    "red": "#ff5d5d",
    "purple": "#b78cff",
    "text": "#f3fff1",
}


def format_preview_rows(rider: dict[str, Any] | None) -> tuple[str, str, str]:
    if not rider:
        return "--:--.--", "P -- L--", "--- MPH"
    last_lap = str(rider.get("last_lap") or "--:--.---")
    position = str(rider.get("position") or "P--")
    bike = str(rider.get("bike_number") or "--")
    speed = str(rider.get("speed_trap") or "--- MPH")
    return last_lap[:8], f"{position[:3]} L--"[:8], speed[:8]


def sector_color(value: str) -> str:
    value = value.strip().lower()
    if not value:
        return COLORS["text"]
    if value.startswith("+"):
        return COLORS["amber"]
    if value.startswith("-"):
        return COLORS["green"]
    return COLORS["purple"]
