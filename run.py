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
    parser.add_argument("--mode", choices=("server", "pc-app", "all"), default="server")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--ssl", action="store_true", help="Serve over HTTPS with a local self-signed certificate")
    args = parser.parse_args()

    if args.mode == "pc-app":
        run([python_executable(), "-m", "apps.pc_pitboard_app", "--api", f"http://127.0.0.1:{args.port}"])
        return 0

    if args.mode == "all":
        server = subprocess.Popen(
            [python_executable(), "-m", "uvicorn", "backend.server:app", "--host", args.host, "--port", str(args.port)],
            cwd=str(ROOT),
        )
        try:
            run([python_executable(), "-m", "apps.pc_pitboard_app", "--api", f"http://127.0.0.1:{args.port}"])
        finally:
            server.terminate()
            server.wait()
        return 0

    if args.ssl:
        cert = ROOT / "backend" / "certs" / "cert.pem"
        key = ROOT / "backend" / "certs" / "key.pem"
        if not cert.exists() or not key.exists():
            print("Creating local HTTPS certificate...")
            run([python_executable(), "-m", "backend.generate_cert", "--output", str(cert.parent), "--host", "localhost", "--host", "127.0.0.1"])
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
