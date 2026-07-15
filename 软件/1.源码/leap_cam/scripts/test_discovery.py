#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import socket
import sys
import time
from dataclasses import dataclass
from typing import Any
from urllib.error import URLError
from urllib.request import Request, urlopen


DISCOVERY_PORT = 33333
HTTP_TIMEOUT = 3.0

MAVLINK_V1_MAGIC = 0xFE
MAVLINK_V2_MAGIC = 0xFD
MAVLINK_MSG_ID_HEARTBEAT = 0
MAVLINK_MSG_ID_STATUSTEXT = 253
MAVLINK_MSG_ID_HEARTBEAT_CRC_EXTRA = 50
MAV_TYPE_GCS = 6
MAV_AUTOPILOT_INVALID = 8
MAV_STATE_ACTIVE = 4


@dataclass(frozen=True)
class MavlinkFrame:
    msg_id: int
    sys_id: int
    comp_id: int
    payload: bytes


@dataclass(frozen=True)
class Device:
    response_from: tuple[str, int]
    payload: dict[str, Any]

    @property
    def name(self) -> str:
        return str(self.payload.get("name", ""))

    @property
    def ip(self) -> str:
        return str(self.payload.get("ip", self.response_from[0]))

    @property
    def handshake_url(self) -> str:
        return str(self.payload.get("handshake_url") or f"http://{self.ip}/handshake")

    @property
    def stream_url(self) -> str:
        return str(self.payload.get("stream_url") or f"http://{self.ip}:81/")


def mavlink_crc_accumulate(data: int, crc: int) -> int:
    data ^= crc & 0xFF
    data ^= (data << 4) & 0xFF
    return (((data << 8) & 0xFFFF) | (crc >> 8)) ^ (data >> 4) ^ ((data << 3) & 0xFFFF)


def mavlink_crc(buffer: bytes, crc_extra: int) -> int:
    crc = 0xFFFF
    for byte in buffer:
        crc = mavlink_crc_accumulate(byte, crc)
    return mavlink_crc_accumulate(crc_extra, crc)


def pack_mavlink_v2(
    sequence: int,
    sys_id: int,
    comp_id: int,
    msg_id: int,
    payload: bytes,
    crc_extra: int,
) -> bytes:
    header = bytes((
        len(payload),
        0,
        0,
        sequence & 0xFF,
        sys_id & 0xFF,
        comp_id & 0xFF,
        msg_id & 0xFF,
        (msg_id >> 8) & 0xFF,
        (msg_id >> 16) & 0xFF,
    ))
    crc = mavlink_crc(header + payload, crc_extra)
    return bytes((MAVLINK_V2_MAGIC,)) + header + payload + bytes((crc & 0xFF, crc >> 8))


def build_gcs_heartbeat(sequence: int = 0) -> bytes:
    payload = bytearray(9)
    payload[4] = MAV_TYPE_GCS
    payload[5] = MAV_AUTOPILOT_INVALID
    payload[6] = 0
    payload[7] = MAV_STATE_ACTIVE
    payload[8] = 3
    return pack_mavlink_v2(
        sequence=sequence,
        sys_id=255,
        comp_id=0,
        msg_id=MAVLINK_MSG_ID_HEARTBEAT,
        payload=bytes(payload),
        crc_extra=MAVLINK_MSG_ID_HEARTBEAT_CRC_EXTRA,
    )


def parse_mavlink_frames(data: bytes) -> list[MavlinkFrame]:
    frames: list[MavlinkFrame] = []
    index = 0

    while index < len(data):
        magic = data[index]

        if magic == MAVLINK_V1_MAGIC:
            if index + 8 > len(data):
                break
            payload_len = data[index + 1]
            frame_len = 6 + payload_len + 2
            if index + frame_len > len(data):
                break
            sys_id = data[index + 3]
            comp_id = data[index + 4]
            msg_id = data[index + 5]
            payload = data[index + 6:index + 6 + payload_len]
            frames.append(MavlinkFrame(msg_id=msg_id, sys_id=sys_id, comp_id=comp_id, payload=payload))
            index += frame_len
            continue

        if magic == MAVLINK_V2_MAGIC:
            if index + 12 > len(data):
                break
            payload_len = data[index + 1]
            incompat_flags = data[index + 2]
            signature_len = 13 if incompat_flags & 0x01 else 0
            frame_len = 10 + payload_len + 2 + signature_len
            if index + frame_len > len(data):
                break
            sys_id = data[index + 5]
            comp_id = data[index + 6]
            msg_id = data[index + 7] | (data[index + 8] << 8) | (data[index + 9] << 16)
            payload = data[index + 10:index + 10 + payload_len]
            frames.append(MavlinkFrame(msg_id=msg_id, sys_id=sys_id, comp_id=comp_id, payload=payload))
            index += frame_len
            continue

        index += 1

    return frames


def decode_statustext(payload: bytes) -> str:
    if len(payload) < 51:
        return ""
    text = payload[1:51].split(b"\0", 1)[0]
    return text.decode("utf-8", errors="replace")


def apply_statustext(payload: dict[str, Any], text: str, fallback_ip: str) -> None:
    key, sep, value = text.partition(":")
    if not sep:
        return

    if key == "DEVICE_NAME":
        payload["name"] = value
    elif key == "DEVICE_IP":
        payload["ip"] = value or fallback_ip
    elif key == "DEVICE_TYPE":
        payload["type"] = value
    elif key == "HTTP_PORT":
        payload["http_port"] = int(value)
    elif key == "STREAM_PORT":
        payload["stream_port"] = int(value)

    ip = str(payload.get("ip") or fallback_ip)
    http_port = int(payload.get("http_port") or 80)
    stream_port = int(payload.get("stream_port") or 81)
    http_host = ip if http_port == 80 else f"{ip}:{http_port}"
    payload["handshake_url"] = f"http://{http_host}/handshake"
    payload["stream_url"] = f"http://{ip}:{stream_port}/"


def discover_devices(
    broadcast_addr: str,
    port: int,
    timeout: float,
    retries: int,
    bind_addr: str,
) -> list[Device]:
    devices: dict[str, Device] = {}

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((bind_addr, port))
        sock.settimeout(0.2)

        target = (broadcast_addr, port)
        sequence = 0
        for attempt in range(1, retries + 1):
            sock.sendto(build_gcs_heartbeat(sequence), target)
            sequence = (sequence + 1) & 0xFF
            deadline = time.monotonic() + timeout

            while time.monotonic() < deadline:
                try:
                    data, addr = sock.recvfrom(2048)
                except socket.timeout:
                    continue

                key = addr[0]
                device = devices.get(key)
                if device is None:
                    device = Device(response_from=addr, payload={"ip": addr[0]})
                    devices[key] = device

                for frame in parse_mavlink_frames(data):
                    if frame.msg_id == MAVLINK_MSG_ID_HEARTBEAT:
                        device.payload["mavlink_sys_id"] = frame.sys_id
                        device.payload["mavlink_comp_id"] = frame.comp_id
                    elif frame.msg_id == MAVLINK_MSG_ID_STATUSTEXT:
                        apply_statustext(device.payload, decode_statustext(frame.payload), addr[0])

            if any(device.payload.get("type") == "maturo_esp32s3_cam" for device in devices.values()):
                break

            print(f"No camera MAVLink discovery after attempt {attempt}/{retries}", file=sys.stderr)

    return [
        device
        for device in devices.values()
        if device.payload.get("type") == "maturo_esp32s3_cam"
    ]


def fetch_json(url: str, timeout: float) -> dict[str, Any]:
    request = Request(
        url,
        method="GET",
        headers={
            "Accept": "application/json",
            "User-Agent": "maturo-cam-discovery-test/1.0",
        },
    )

    with urlopen(request, timeout=timeout) as response:
        body = response.read().decode("utf-8", errors="replace")
        return json.loads(body)


def test_stream(url: str, timeout: float) -> str:
    request = Request(
        url,
        method="GET",
        headers={
            "User-Agent": "maturo-cam-discovery-test/1.0",
        },
    )

    with urlopen(request, timeout=timeout) as response:
        content_type = response.headers.get("Content-Type", "")
        chunk = response.read(64)

    if "multipart/x-mixed-replace" not in content_type:
        raise RuntimeError(f"Unexpected stream content type: {content_type}")
    if not chunk:
        raise RuntimeError("Stream connected but no bytes were received")

    return content_type


def print_device(device: Device, handshake: dict[str, Any] | None, stream_status: str | None) -> None:
    print(f"Device: {device.name or '(unnamed)'}")
    print(f"  MAVLink from: {device.response_from[0]}:{device.response_from[1]}")
    print(f"  IP: {device.ip}")
    print(f"  MAVLink sys/comp: {device.payload.get('mavlink_sys_id', '')}/{device.payload.get('mavlink_comp_id', '')}")
    print(f"  Handshake URL: {device.handshake_url}")
    print(f"  Stream URL: {device.stream_url}")

    if handshake is not None:
        print("  HTTP handshake: OK")
        print(f"  HTTP name: {handshake.get('name', '')}")
        print(f"  HTTP ip: {handshake.get('ip', '')}")

    if stream_status is not None:
        print(f"  Stream: OK ({stream_status})")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Discover and test Maturo ESP32-S3 camera devices using MAVLink-style discovery."
    )
    parser.add_argument("--broadcast", default="255.255.255.255", help="UDP broadcast address")
    parser.add_argument("--bind", default="0.0.0.0", help="Local bind address")
    parser.add_argument("--port", type=int, default=DISCOVERY_PORT, help="UDP MAVLink discovery port")
    parser.add_argument("--timeout", type=float, default=1.5, help="Seconds to wait per broadcast")
    parser.add_argument("--retries", type=int, default=3, help="Broadcast attempts")
    parser.add_argument("--http-timeout", type=float, default=HTTP_TIMEOUT, help="HTTP timeout seconds")
    parser.add_argument("--no-http", action="store_true", help="Only run MAVLink discovery")
    parser.add_argument("--stream", action="store_true", help="Also connect to MJPEG stream")
    parser.add_argument("--json", action="store_true", help="Print raw JSON list")
    return parser


def main() -> int:
    args = build_parser().parse_args()

    devices = discover_devices(
        broadcast_addr=args.broadcast,
        port=args.port,
        timeout=args.timeout,
        retries=args.retries,
        bind_addr=args.bind,
    )

    results: list[dict[str, Any]] = []
    if not devices:
        print("No Maturo ESP32-S3 camera device discovered.", file=sys.stderr)
        return 1

    exit_code = 0
    for device in devices:
        handshake = None
        stream_status = None
        errors: list[str] = []

        if not args.no_http:
            try:
                handshake = fetch_json(device.handshake_url, timeout=args.http_timeout)
            except (URLError, TimeoutError, json.JSONDecodeError, OSError) as exc:
                errors.append(f"HTTP handshake failed: {exc}")
                exit_code = 2

        if args.stream:
            try:
                stream_status = test_stream(device.stream_url, timeout=args.http_timeout)
            except (URLError, TimeoutError, OSError, RuntimeError) as exc:
                errors.append(f"Stream test failed: {exc}")
                exit_code = 3

        if args.json:
            results.append({
                "mavlink": device.payload,
                "response_from": {
                    "ip": device.response_from[0],
                    "port": device.response_from[1],
                },
                "handshake": handshake,
                "stream_status": stream_status,
                "errors": errors,
            })
            continue

        print_device(device, handshake, stream_status)
        for error in errors:
            print(f"  ERROR: {error}", file=sys.stderr)

    if args.json:
        print(json.dumps(results, ensure_ascii=False, indent=2))

    return exit_code


if __name__ == "__main__":
    raise SystemExit(main())
