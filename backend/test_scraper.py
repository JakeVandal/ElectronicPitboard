from __future__ import annotations

from backend.scraper import parse_race_html

THREE_SECTOR_HTML = """
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

FIVE_SECTOR_HTML = """
<html><body>
<div id="server-load">68</div>
<table>
  <thead>
    <tr>
      <th>Pos</th>
      <th>Bike</th>
      <th>Name</th>
      <th>Last</th>
      <th>Best</th>
      <th>S1</th>
      <th>S2</th>
      <th>S3</th>
      <th>S4</th>
      <th>S5</th>
      <th>Trap</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>P2</td>
      <td>21</td>
      <td>Jamie Ross</td>
      <td>1:25.982</td>
      <td>1:25.120</td>
      <td>28.301</td>
      <td>32.118</td>
      <td>25.951</td>
      <td>24.154</td>
      <td>22.178</td>
      <td>171.3 mph</td>
    </tr>
  </tbody>
</table>
</body></html>
"""


def test_parse_three_sector_html() -> None:
    payload = parse_race_html(THREE_SECTOR_HTML)
    assert payload["server_load"] == 42
    assert payload["sector_count"] == 3
    assert payload["riders"][0]["position"] == "P1"
    assert payload["riders"][0]["bike_number"] == 5
    assert payload["riders"][0]["last_lap"] == "1:24.582"
    assert payload["riders"][0]["best_lap"] == "1:23.901"
    assert payload["riders"][0]["sectors"]["S1"] == "27.403"
    assert payload["riders"][0]["speed_trap"] == "168.4 mph"


def test_parse_five_sector_html() -> None:
    payload = parse_race_html(FIVE_SECTOR_HTML)
    assert payload["server_load"] == 68
    assert payload["sector_count"] == 5
    assert payload["riders"][0]["bike_number"] == 21
    assert payload["riders"][0]["sectors"]["S4"] == "24.154"
    assert payload["riders"][0]["sectors"]["S5"] == "22.178"
    assert payload["riders"][0]["speed_trap"] == "171.3 mph"
