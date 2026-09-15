from __future__ import annotations

import asyncio
import re
from typing import Any

import httpx
from bs4 import BeautifulSoup

TIMING_URL = "http://timing.motoamerica.com/timing_frame.php"


def _clean_text(value: Any) -> str:
    if value is None:
        return ""
    text = str(value).strip()
    return re.sub(r"\s+", " ", text)


def _extract_server_load(text: str, html: str | None = None) -> int:
    patterns = [
        r"server-load[^\d]*(\d+)",
        r"id\s*=\s*['\"]server-load['\"][^>]*>\s*(\d+)",
        r"id\s*=\s*['\"]server-load['\"][^>]*\s*/?>",
    ]
    for pattern in patterns:
        match = re.search(pattern, text, flags=re.IGNORECASE | re.DOTALL)
        if match:
            if pattern.endswith(r"\s*/?>"):
                next_match = re.search(r">\s*(\d+)", html or text, flags=re.IGNORECASE | re.DOTALL)
                if next_match:
                    return int(next_match.group(1))
            return int(match.group(1))
    if html:
        match = re.search(r"server-load[^\d]*(\d+)", html, flags=re.IGNORECASE | re.DOTALL)
        if match:
            return int(match.group(1))
    return 0


def _normalize_rider_number(value: Any) -> int | None:
    text = _clean_text(value)
    if not text:
        return None
    match = re.search(r"(\d+)", text)
    if not match:
        return None
    return int(match.group(1))


def _extract_sector_map(headers: list[str], row: list[str]) -> dict[str, str]:
    sectors: dict[str, str] = {}
    for idx, header in enumerate(headers):
        header_name = header.strip().upper()
        if re.fullmatch(r"S\d+", header_name):
            sectors[header_name] = _clean_text(row[idx]) if idx < len(row) else ""
    return sectors


def parse_race_html(html: str) -> dict[str, Any]:
    soup = BeautifulSoup(html, "html.parser")
    server_load = _extract_server_load(soup.get_text(" ", strip=False), html)

    table = soup.find("table")
    riders: list[dict[str, Any]] = []
    if table is None:
        return {"server_load": server_load, "sector_count": 0, "riders": riders}

    headers = []
    for th in table.select("thead th"):
        headers.append(_clean_text(th.get_text(" ", strip=True)))

    if not headers:
        headers = [_clean_text(th.get_text(" ", strip=True)) for th in table.select("tr:first-child th")]

    sector_headers = [h for h in headers if re.fullmatch(r"S\d+", h.upper())]
    sector_count = max([len(sector_headers), 0])

    for row in table.select("tbody tr"):
        cells = [_clean_text(cell.get_text(" ", strip=True)) for cell in row.select("td")]
        if not cells:
            continue

        row_map = {header: cells[idx] if idx < len(cells) else "" for idx, header in enumerate(headers[: len(cells)])}
        raw_position = row_map.get("Pos", row_map.get("Position", cells[0] if cells else ""))
        bike_number = _normalize_rider_number(row_map.get("Bike", row_map.get("Bike #", row_map.get("#", cells[1] if len(cells) > 1 else ""))))
        rider_name = row_map.get("Name", row_map.get("Rider", "")) or ""
        last_lap = row_map.get("Last", row_map.get("Last Lap", ""))
        best_lap = row_map.get("Best", row_map.get("Best Lap", ""))
        speed_trap = row_map.get("Speed", row_map.get("Trap", row_map.get("Top Speed", row_map.get("Speed Trap", ""))))

        if bike_number is None:
            continue

        sectors = _extract_sector_map(headers, cells)
        riders.append(
            {
                "position": raw_position,
                "bike_number": bike_number,
                "rider_name": rider_name,
                "last_lap": last_lap,
                "best_lap": best_lap,
                "sectors": sectors,
                "speed_trap": speed_trap,
            }
        )

    return {"server_load": server_load, "sector_count": sector_count, "riders": riders}


async def fetch_timing_frame(url: str = TIMING_URL) -> dict[str, Any]:
    async with httpx.AsyncClient(timeout=15.0) as client:
        response = await client.get(url)
        response.raise_for_status()
        return parse_race_html(response.text)


async def poll_timing() -> None:
    while True:
        await fetch_timing_frame()
        await asyncio.sleep(1)


if __name__ == "__main__":
    import json

    sample_html = """
    <html><body>
    <div id="server-load">42</div>
    <table>
      <thead>
        <tr>
          <th>Pos</th>
          <th>#</th>
          <th>Name</th>
          <th>Last</th>
          <th>Best</th>
          <th>S1</th>
          <th>S2</th>
          <th>S3</th>
          <th>Speed</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td>P1</td>
          <td>5</td>
          <td>Alex Rider</td>
          <td>1:24.582</td>
          <td>1:23.901</td>
          <td>27.403</td>
          <td>31.145</td>
          <td>25.908</td>
          <td>168.4 mph</td>
        </tr>
      </tbody>
    </table>
    </body></html>
    """
    print(json.dumps(parse_race_html(sample_html), indent=2))
