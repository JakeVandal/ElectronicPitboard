from __future__ import annotations

import asyncio
from typing import Any
from pathlib import Path

from fastapi import FastAPI, WebSocket
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse

from backend.scraper import fetch_timing_frame

app = FastAPI(title="Pitboard Timing Hub")
WEBAPP_DIR = Path(__file__).resolve().parent.parent / "webapp"

LIVE_DATA: dict[str, Any] = {
    "server_load": 0,
    "sector_count": 0,
    "riders": [],
}


@app.get("/api/session")
async def session() -> dict[str, Any]:
    return {
        "server_load": LIVE_DATA.get("server_load", 0),
        "sector_count": LIVE_DATA.get("sector_count", 0),
        "rider_count": len(LIVE_DATA.get("riders", [])),
    }


@app.get("/api/riders")
async def riders() -> list[dict[str, Any]]:
    return LIVE_DATA.get("riders", [])


@app.get("/api/rider/{number}")
async def rider(number: int) -> dict[str, Any]:
    for rider in LIVE_DATA.get("riders", []):
        if int(rider.get("bike_number")) == int(number):
            return rider
    return {"bike_number": number, "error": "not_found"}


@app.websocket("/ws/live")
async def websocket_live(websocket: WebSocket) -> None:
    await websocket.accept()
    while True:
        try:
            await websocket.send_json(LIVE_DATA)
            await asyncio.sleep(1.0)
        except Exception:
            break


@app.on_event("startup")
async def startup_event() -> None:
    async def refresh_loop() -> None:
        while True:
            try:
                global LIVE_DATA
                LIVE_DATA = await fetch_timing_frame()
            except Exception:
                LIVE_DATA = {"server_load": 0, "sector_count": 0, "riders": []}
            await asyncio.sleep(1.0)

    asyncio.create_task(refresh_loop())


@app.get("/")
async def index() -> FileResponse:
    return FileResponse(WEBAPP_DIR / "index.html")


app.mount("/static", StaticFiles(directory=WEBAPP_DIR), name="static")
app.mount("/", StaticFiles(directory=WEBAPP_DIR, html=True), name="webapp")
