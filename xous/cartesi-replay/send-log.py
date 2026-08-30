#!/usr/bin/env python3
"""Send one framed Cartesi step log to Baosec and receive its journal."""

import argparse
import struct
import sys

import serial

JOURNAL_BYTES = 96
READY_MESSAGE = b"CARTESI_READY\n"


def read_exact(port: serial.Serial, size: int) -> bytes:
    output = bytearray()
    while len(output) < size:
        chunk = port.read(size - len(output))
        if not chunk:
            raise TimeoutError(f"timed out after receiving {len(output)} of {size} journal bytes")
        output.extend(chunk)
    return bytes(output)


def wait_ready(port: serial.Serial) -> None:
    received = bytearray()
    while not received.endswith(READY_MESSAGE):
        chunk = port.read(1)
        if not chunk:
            raise TimeoutError("timed out waiting for CARTESI_READY")
        received.extend(chunk)
        if len(received) > len(READY_MESSAGE):
            del received[:-len(READY_MESSAGE)]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="USB CDC serial device, for example /dev/ttyACM0")
    parser.add_argument("input", nargs="?", default="-", help="step log file (default: stdin)")
    parser.add_argument("output", nargs="?", default="-", help="journal file (default: stdout)")
    parser.add_argument("--baud", type=int, default=1_000_000)
    parser.add_argument("--timeout", type=float, default=120.0)
    args = parser.parse_args()

    source = sys.stdin.buffer if args.input == "-" else open(args.input, "rb")
    try:
        log = source.read()
    finally:
        if source is not sys.stdin.buffer:
            source.close()
    if not log:
        parser.error("step log is empty")

    with serial.Serial(args.port, args.baud, timeout=args.timeout, write_timeout=args.timeout) as port:
        wait_ready(port)
        port.write(struct.pack("<Q", len(log)))
        port.write(log)
        port.flush()
        journal = read_exact(port, JOURNAL_BYTES)

    destination = sys.stdout.buffer if args.output == "-" else open(args.output, "wb")
    try:
        destination.write(journal)
        destination.flush()
    finally:
        if destination is not sys.stdout.buffer:
            destination.close()


if __name__ == "__main__":
    main()
