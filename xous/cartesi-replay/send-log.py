#!/usr/bin/env python3
"""Send one framed Cartesi step log to Baosec and receive its journal."""

import argparse
import os
import struct
import sys
import time

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


def wait_for_port(path: str, timeout: float, verbose: bool = False) -> None:
    if os.path.exists(path):
        return
    if verbose:
        print(f"waiting for {path} to appear", file=sys.stderr, flush=True)
    deadline = time.monotonic() + timeout
    while not os.path.exists(path):
        if time.monotonic() >= deadline:
            raise TimeoutError(f"timed out waiting for {path} to appear")
        time.sleep(0.1)
    if verbose:
        print(f"found {path}", file=sys.stderr, flush=True)


def wait_ready(port: serial.Serial, verbose: bool = False) -> None:
    if verbose:
        print("waiting for CARTESI_READY", file=sys.stderr, flush=True)
    received = bytearray()
    while not received.endswith(READY_MESSAGE):
        chunk = port.read(1)
        if not chunk:
            raise TimeoutError("timed out waiting for CARTESI_READY")
        received.extend(chunk)
        if len(received) > len(READY_MESSAGE):
            del received[:-len(READY_MESSAGE)]
    if verbose:
        print("received CARTESI_READY", file=sys.stderr, flush=True)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="USB CDC serial device, for example /dev/ttyACM0")
    parser.add_argument("input", nargs="?", default="-", help="step log file (default: stdin)")
    parser.add_argument("output", nargs="?", default="-", help="journal file (default: stdout)")
    parser.add_argument("--baud", type=int, default=1_000_000)
    parser.add_argument("--timeout", type=float, default=120.0)
    parser.add_argument("-v", "--verbose", action="store_true", help="print transfer progress to stderr")
    args = parser.parse_args()

    source = sys.stdin.buffer if args.input == "-" else open(args.input, "rb")
    try:
        log = source.read()
    finally:
        if source is not sys.stdin.buffer:
            source.close()
    if not log:
        parser.error("step log is empty")

    wait_for_port(args.port, args.timeout, args.verbose)
    if args.verbose:
        print(f"opening {args.port}; step log is {len(log)} bytes", file=sys.stderr, flush=True)
    with serial.Serial(args.port, args.baud, timeout=args.timeout, write_timeout=args.timeout) as port:
        wait_ready(port, args.verbose)
        frame = struct.pack("<Q", len(log)) + log
        port.write(frame)
        port.flush()
        if args.verbose:
            print(f"sent {len(frame)} bytes; waiting for {JOURNAL_BYTES}-byte journal", file=sys.stderr, flush=True)
        journal = read_exact(port, JOURNAL_BYTES)
        if args.verbose:
            print("received journal", file=sys.stderr, flush=True)

    destination = sys.stdout.buffer if args.output == "-" else open(args.output, "wb")
    try:
        destination.write(journal)
        destination.flush()
    finally:
        if destination is not sys.stdout.buffer:
            destination.close()
    if args.verbose:
        print(f"wrote {len(journal)} bytes to {args.output}", file=sys.stderr, flush=True)


if __name__ == "__main__":
    main()
