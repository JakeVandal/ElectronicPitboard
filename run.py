#!/usr/bin/env python3
"""Boot the timing backend and HTTPS bridge helper."""

from __future__ import annotations

import argparse
import signal
import subprocess
import sys
import time
from urllib.error import URLError
from urllib.request import urlopen
from pathlib import Path

ROOT = Path(__file__).resolve().parent
VENV_PYTHON = ROOT / ".venv" / "bin" / "python"
WINDOWS_VENV_PYTHON = ROOT / "venv" / "Scripts" / "python.exe"
WINDOWS_DOT_VENV_PYTHON = ROOT / ".venv" / "Scripts" / "python.exe"
POSIX_VENV_PYTHON = ROOT / "venv" / "bin" / "python"


def python_executable() -> str:
    for candidate in (VENV_PYTHON, POSIX_VENV_PYTHON, WINDOWS_DOT_VENV_PYTHON, WINDOWS_VENV_PYTHON):
        if candidate.is_file():
            return str(candidate)
    return sys.executable


def run(cmd: list[str]) -> None:
    print("+", " ".join(cmd))
    subprocess.run(cmd, cwd=str(ROOT), check=True)


def wait_for_backend(host: str, port: int, timeout: float = 30.0) -> None:
    deadline = time.monotonic() + timeout
    url = f"http://127.0.0.1:{port}/api/session"
    while time.monotonic() < deadline:
        try:
            with urlopen(url, timeout=1) as response:
                if response.status == 200:
                    print(f"Backend ready at http://{host}:{port}")
                    return
        except (OSError, URLError):
            time.sleep(0.25)
    raise RuntimeError(f"Backend did not become ready at {url}")


def terminate_process(process: subprocess.Popen[object] | None) -> None:
    if process is None or process.poll() is not None:
        return
    if sys.platform == "win32":
        process.send_signal(signal.CTRL_BREAK_EVENT)
    else:
        process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait()


def run_dev(host: str, port: int) -> int:
    if sys.platform != "win32":
        def handle_sigterm(_signum: int, _frame: object) -> None:
            raise KeyboardInterrupt

        signal.signal(signal.SIGTERM, handle_sigterm)
    interpreter = python_executable()
    backend = subprocess.Popen(
        [interpreter, "-m", "uvicorn", "backend.server:app", "--host", host, "--port", str(port)],
        cwd=str(ROOT),
        start_new_session=sys.platform != "win32",
    )
    gui: subprocess.Popen[object] | None = None
    try:
        wait_for_backend(host, port)
        gui = subprocess.Popen(
            [interpreter, "-m", "apps.pc_pitboard_app", "--api", f"http://127.0.0.1:{port}"],
            cwd=str(ROOT),
            creationflags=subprocess.CREATE_NEW_PROCESS_GROUP if sys.platform == "win32" else 0,
        )
        print("PC Bluetooth app started. Press Ctrl+C to stop the full stack.")
        while True:
            if backend.poll() is not None:
                raise RuntimeError(f"Backend exited with status {backend.returncode}")
            if gui.poll() is not None:
                print("PC Bluetooth app exited; stopping backend.")
                return gui.returncode or 0
            time.sleep(0.25)
    finally:
        terminate_process(gui)
        terminate_process(backend)


def main() -> int:
    parser = argparse.ArgumentParser(description="Run the pitboard timing service")
    parser.add_argument("--mode", choices=("server", "pc-app", "all", "dev"), default="server")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--ssl", action="store_true", help="Serve over HTTPS with a local self-signed certificate")
    args = parser.parse_args()

    if args.mode == "dev":
        return run_dev(args.host, args.port)

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
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        print("\nPitboard server stopped.")
        raise SystemExit(130)
