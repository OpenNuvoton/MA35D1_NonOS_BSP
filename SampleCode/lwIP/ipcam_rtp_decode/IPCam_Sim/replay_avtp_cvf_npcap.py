#!/usr/bin/env python3
"""Replay a concatenated AVTP/CVF H.264 stream through Npcap.

The input file contains AVTPDUs produced by GStreamer's avtpcvfpay and
filesink. It does not contain Ethernet headers. Each AVTPDU is wrapped in an
Ethernet header with EtherType 0x22F0 before transmission.
"""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path
from typing import Iterator

try:
    from scapy.all import Ether, get_if_list, get_if_hwaddr, sendp
except ImportError as exc:
    print("Scapy is required. Install it with: python -m pip install scapy", file=sys.stderr)
    raise SystemExit(2) from exc


AVTP_ETHERTYPE = 0x22F0
AVTP_SUBTYPE_CVF = 0x03
AVTP_HEADER_SIZE = 24
CVF_H264_PAYLOAD_HEADER_SIZE = 4
DEFAULT_FPS = 50.0
DEFAULT_STREAM_ID = bytes.fromhex("45 4d 4f 53 89 ab cd ef")


class AvtpFormatError(ValueError):
    """Raised when the input is not a valid concatenated AVTP/CVF stream."""


def iter_avtpdus(data: bytes, expected_stream_id: bytes | None = None) -> Iterator[tuple[bytes, int, bool]]:
    """Yield (AVTPDU, sequence number, marker) tuples from a binary stream."""
    offset = 0
    data_size = len(data)

    while offset < data_size:
        remaining = data_size - offset
        if remaining < AVTP_HEADER_SIZE:
            raise AvtpFormatError(
                f"truncated AVTP header at offset {offset} ({remaining} bytes remain)"
            )

        header = data[offset : offset + AVTP_HEADER_SIZE]
        subtype = header[0] & 0x7F
        if subtype != AVTP_SUBTYPE_CVF:
            raise AvtpFormatError(
                f"unexpected AVTP subtype 0x{subtype:02x} at offset {offset}"
            )

        stream_id = header[4:12]
        if expected_stream_id is not None and stream_id != expected_stream_id:
            raise AvtpFormatError(
                f"unexpected Stream ID {stream_id.hex(':')} at offset {offset}"
            )

        packet_info = int.from_bytes(header[20:24], "big")
        stream_data_length = (packet_info >> 16) & 0xFFFF
        if stream_data_length < CVF_H264_PAYLOAD_HEADER_SIZE:
            raise AvtpFormatError(
                f"invalid CVF payload length {stream_data_length} at offset {offset}"
            )

        avtpdu_length = AVTP_HEADER_SIZE + stream_data_length
        if offset + avtpdu_length > data_size:
            raise AvtpFormatError(
                f"truncated AVTPDU at offset {offset}: "
                f"need {avtpdu_length} bytes, have {remaining}"
            )

        sequence_number = header[2]
        marker = bool((packet_info >> 12) & 0x01)
        yield data[offset : offset + avtpdu_length], sequence_number, marker
        offset += avtpdu_length


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Replay concatenated AVTP/CVF H.264 AVTPDUs via Npcap."
    )
    parser.add_argument(
        "--input",
        type=Path,
        required=True,
        help="input AVTPDU file",
    )
    parser.add_argument(
        "--iface",
        help="Npcap interface name; use --list-interfaces to show available names",
    )
    parser.add_argument(
        "--list-interfaces",
        action="store_true",
        help="list Npcap interfaces and exit",
    )
    parser.add_argument(
        "--dst-mac",
        default="ff:ff:ff:ff:ff:ff",
        help="Ethernet destination MAC (default: broadcast)",
    )
    parser.add_argument(
        "--src-mac",
        help="Ethernet source MAC (default: selected interface MAC)",
    )
    parser.add_argument(
        "--stream-id",
        default=DEFAULT_STREAM_ID.hex(),
        help="expected AVTP Stream ID as 16 hex digits; use 'any' to disable checking",
    )
    parser.add_argument(
        "--fps",
        type=float,
        default=DEFAULT_FPS,
        help=f"frame rate used for replay pacing (default: {DEFAULT_FPS:g})",
    )
    parser.add_argument(
        "--speed",
        type=float,
        default=1.0,
        help="replay speed multiplier (default: 1.0)",
    )
    parser.add_argument(
        "--unpaced",
        "--no-pacing",
        dest="unpaced",
        action="store_true",
        help="send AVTPDUs continuously without frame pacing",
    )
    parser.add_argument(
        "--loop",
        action="store_true",
        help="restart the file after the last AVTPDU",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="validate and report the stream without transmitting",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="print each AVTPDU transmission",
    )
    return parser.parse_args()


def format_interface_list(interfaces: list[str]) -> str:
    lines = []
    for index, interface in enumerate(interfaces):
        try:
            mac = get_if_hwaddr(interface)
        except Exception:
            mac = "unknown"
        lines.append(f"[{index}] {interface}  MAC={mac}")
    return "\n".join(lines)


def main() -> int:
    args = parse_args()
    interfaces = get_if_list()

    if args.list_interfaces:
        print(format_interface_list(interfaces))
        return 0

    if not args.input.is_file():
        print(f"Input file not found: {args.input}", file=sys.stderr)
        return 1
    if not args.unpaced and args.fps <= 0.0:
        print("--fps must be greater than zero", file=sys.stderr)
        return 1
    if not args.unpaced and args.speed <= 0.0:
        print("--speed must be greater than zero", file=sys.stderr)
        return 1
    if not args.dry_run and not args.iface:
        print("--iface is required for transmission; use --list-interfaces", file=sys.stderr)
        return 1
    if args.iface and args.iface not in interfaces:
        print(f"Interface not found: {args.iface}", file=sys.stderr)
        print(format_interface_list(interfaces), file=sys.stderr)
        return 1

    try:
        dst_mac = args.dst_mac
        stream_id = None if args.stream_id.lower() == "any" else bytes.fromhex(args.stream_id)
        if stream_id is not None and len(stream_id) != 8:
            raise ValueError("Stream ID must contain exactly 8 bytes")
        data = args.input.read_bytes()
        packets = list(iter_avtpdus(data, stream_id))
    except (OSError, ValueError, AvtpFormatError) as exc:
        print(f"Input validation failed: {exc}", file=sys.stderr)
        return 1

    frame_count = sum(1 for _, _, marker in packets if marker)
    payload_bytes = sum(len(avtpdu) - AVTP_HEADER_SIZE for avtpdu, _, _ in packets)
    print(f"Input       : {args.input}")
    print(f"AVTPDUs     : {len(packets)}")
    print(f"Frames      : {frame_count}")
    print(f"AVTP bytes  : {len(data)}")
    print(
        "H.264 bytes : "
        f"{payload_bytes - len(packets) * CVF_H264_PAYLOAD_HEADER_SIZE}"
    )
    print(f"Stream ID   : {args.stream_id}")
    print(f"Destination : {dst_mac}")
    if args.unpaced:
        print("Pacing      : disabled (maximum send rate)")
    else:
        print(f"Pacing      : {args.fps:g} fps, {args.speed:g}x")

    if args.dry_run:
        print("Dry run: no packets transmitted")
        return 0

    src_mac = args.src_mac or get_if_hwaddr(args.iface)
    print(f"Interface   : {args.iface}")
    print(f"Source MAC  : {src_mac}")
    print("Replay started; press Ctrl+C to stop")

    frame_period = 0.0 if args.unpaced else 1.0 / args.fps / args.speed
    sent_packets = 0
    sent_frames = 0

    try:
        while True:
            for avtpdu, sequence_number, marker in packets:
                frame = Ether(dst=dst_mac, src=src_mac, type=AVTP_ETHERTYPE) / avtpdu
                sendp(frame, iface=args.iface, verbose=False)
                sent_packets += 1

                if args.verbose:
                    print(
                        f"sent packet={sent_packets} seq={sequence_number} "
                        f"bytes={len(avtpdu)} marker={int(marker)}"
                    )

                if marker:
                    sent_frames += 1
                    if args.verbose or sent_frames % 30 == 0:
                        print(f"sent frames={sent_frames}, packets={sent_packets}")
                    if not args.unpaced:
                        time.sleep(frame_period)

            if not args.loop:
                break

    except KeyboardInterrupt:
        print("\nReplay stopped")

    print(f"Replay finished: frames={sent_frames}, packets={sent_packets}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
