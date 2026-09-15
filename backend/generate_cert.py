from __future__ import annotations

import argparse
from datetime import datetime, timedelta, timezone
from ipaddress import ip_address
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID


def generate(output_dir: Path, hostnames: list[str]) -> tuple[Path, Path]:
    output_dir.mkdir(parents=True, exist_ok=True)
    key_path = output_dir / "key.pem"
    cert_path = output_dir / "cert.pem"
    key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    subject = issuer = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, hostnames[0])])
    names: list[x509.GeneralName] = []
    for hostname in hostnames:
        try:
            names.append(x509.IPAddress(ip_address(hostname)))
        except ValueError:
            names.append(x509.DNSName(hostname))
    cert = (
        x509.CertificateBuilder()
        .subject_name(subject)
        .issuer_name(issuer)
        .public_key(key.public_key())
        .serial_number(x509.random_serial_number())
        .not_valid_before(datetime.now(timezone.utc) - timedelta(minutes=1))
        .not_valid_after(datetime.now(timezone.utc) + timedelta(days=30))
        .add_extension(x509.SubjectAlternativeName(names), critical=False)
        .sign(key, hashes.SHA256())
    )
    key_path.write_bytes(key.private_bytes(serialization.Encoding.PEM, serialization.PrivateFormat.TraditionalOpenSSL, serialization.NoEncryption()))
    cert_path.write_bytes(cert.public_bytes(serialization.Encoding.PEM))
    return cert_path, key_path


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate local HTTPS certificates")
    parser.add_argument("--output", default="backend/certs")
    parser.add_argument("--host", action="append", dest="hosts")
    args = parser.parse_args()
    cert, key = generate(Path(args.output), args.hosts or ["localhost", "127.0.0.1"])
    print(f"certificate: {cert}")
    print(f"private key: {key}")
