#!/usr/bin/env python3
"""Boot the timing backend and HTTPS bridge helper."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
VENV_PYTHON = ROOT / ".venv" / "bin" / "python"


def python_executable() -> str:
    if VENV_PYTHON.is_file():
        return str(VENV_PYTHON)
    return sys.executable


def run(cmd: list[str]) -> None:
    print("+", " ".join(cmd))
    subprocess.run(cmd, cwd=str(ROOT), check=True)


def main() -> int:
    parser = argparse.ArgumentParser(description="Run the pitboard timing service")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--ssl", action="store_true", help="Serve over HTTPS with a local self-signed certificate")
    args = parser.parse_args()

    if args.ssl:
        cert = ROOT / "localhost.pem"
        key = ROOT / "localhost-key.pem"
        if not cert.exists() or not key.exists():
            print("Creating temporary localhost certificate...")
            run(["mkcert", "localhost", "127.0.0.1", "::1"])
            cert = ROOT / "localhost.pem"
            key = ROOT / "localhost-key.pem"
        run([
            python_executable(),
            "-m",
            "uvicorn",
            "backend.server:app",
            "--host",
            args.host,
            "--port",
            str(args.port),
            "--ssl-keyfile",
            str(key),
            "--ssl-certfile",
            str(cert),
        ])
        return 0

    run([
        python_executable(),
        "-m",
        "uvicorn",
        "backend.server:app",
        "--host",
        args.host,
        "--port",
        str(args.port),
    ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
