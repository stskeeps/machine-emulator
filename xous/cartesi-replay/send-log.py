#!/usr/bin/env python3
"""Send one framed Cartesi step log to Baosec and receive its journal."""

import argparse
import os
import struct
import sys
import time

import serial

JOURNAL_BYTES = 96
HOOK_SETTLE_SECONDS = 0.5
TX_CHUNK_BYTES = 64
TX_CHUNK_DELAY = 0.02
READY_MESSAGE = b"CARTESI_READY\n"
RECEIVED_MESSAGE = b"CARTESI_RECEIVED\n"
JOURNAL_MESSAGE = b"CARTESI_JOURNAL\n"
BOOT_MENU = b"Commands include:"


class BootloaderPrompt(Exception):
    pass


def read_exact(port: serial.Serial, size: int, verbose: bool = False) -> bytes:
    output = bytearray()
    while len(output) < size:
        chunk = port.read(size - len(output))
        if not chunk:
            if verbose:
                print(f"partial journal ({len(output)}/{size}): {output.hex()}", file=sys.stderr, flush=True)
            raise TimeoutError(f"timed out after receiving {len(output)} of {size} journal bytes")
        output.extend(chunk)
        if verbose:
            start = len(output) - len(chunk)
            print(f"journal rx {start}:{len(output)}: {chunk.hex()}", file=sys.stderr, flush=True)
    if verbose:
        print(f"journal: {bytes(output).hex()}", file=sys.stderr, flush=True)
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


def wait_ready(port: serial.Serial, timeout: float, verbose: bool = False) -> None:
    if verbose:
        print("waiting for CARTESI_READY", file=sys.stderr, flush=True)
    received = bytearray()
    deadline = time.monotonic() + timeout
    port.write(b"\n")
    port.flush()
    if verbose:
        print("sending Enter handshake", file=sys.stderr, flush=True)
    while not received.endswith(READY_MESSAGE):
        now = time.monotonic()
        if now >= deadline:
            raise TimeoutError("timed out waiting for CARTESI_READY")
        port.timeout = min(0.2, max(0.01, deadline - now))
        chunk = port.read(1)
        if not chunk:
            continue
        received.extend(chunk)
        if BOOT_MENU in received or b"Command not recognized" in received:
            raise BootloaderPrompt
        if len(received) > 4096:
            del received[:-4096]
    if verbose:
        print("received CARTESI_READY", file=sys.stderr, flush=True)


def wait_marker(port: serial.Serial, marker: bytes, timeout: float, verbose: bool = False) -> None:
    received = bytearray()
    deadline = time.monotonic() + timeout
    while not received.endswith(marker):
        if time.monotonic() >= deadline:
            raise TimeoutError(f"timed out waiting for {marker!r}")
        chunk = port.read(1)
        if not chunk:
            continue
        received.extend(chunk)
        if verbose:
            shown = chr(chunk[0]) if 32 <= chunk[0] < 127 else "."
            print(f"rx marker byte: 0x{chunk[0]:02x} ({shown})", file=sys.stderr, flush=True)
        if len(received) > 4096:
            del received[:-4096]
    if verbose:
        print(f"received {marker.decode().strip()}", file=sys.stderr, flush=True)


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

    frame = struct.pack("<Q", len(log)) + log
    deadline = time.monotonic() + args.timeout
    while True:
        remaining = max(1.0, deadline - time.monotonic())
        wait_for_port(args.port, remaining, args.verbose)
        if args.verbose:
            print(f"opening {args.port}; step log is {len(log)} bytes", file=sys.stderr, flush=True)
        port = serial.Serial(args.port, args.baud, timeout=min(args.timeout, remaining), write_timeout=min(args.timeout, remaining))
        try:
            wait_ready(port, remaining, args.verbose)
            if args.verbose:
                print(f"waiting {HOOK_SETTLE_SECONDS:.1f}s for binary hook", file=sys.stderr, flush=True)
            time.sleep(HOOK_SETTLE_SECONDS)
            port.timeout = min(args.timeout, remaining)
            sent = 0
            while sent < len(frame):
                chunk = frame[sent:sent + TX_CHUNK_BYTES]
                port.write(chunk)
                port.flush()
                sent += len(chunk)
                if args.verbose:
                    print(f"sent {sent}/{len(frame)} bytes", file=sys.stderr, flush=True)
                time.sleep(TX_CHUNK_DELAY)
            if args.verbose:
                print(f"sent {len(frame)} bytes; waiting for CARTESI_RECEIVED", file=sys.stderr, flush=True)
            wait_marker(port, RECEIVED_MESSAGE, remaining, args.verbose)
            if args.verbose:
                print("waiting for CARTESI_JOURNAL", file=sys.stderr, flush=True)
            wait_marker(port, JOURNAL_MESSAGE, remaining, args.verbose)
            if args.verbose:
                print(f"waiting for {JOURNAL_BYTES}-byte journal", file=sys.stderr, flush=True)
            journal = read_exact(port, JOURNAL_BYTES, args.verbose)
            if args.verbose:
                print("received journal", file=sys.stderr, flush=True)
            port.close()
            break
        except BootloaderPrompt:
            if args.verbose:
                print("bootloader menu detected; sending boot", file=sys.stderr, flush=True)
            port.write(b"boot\n")
            port.flush()
            port.close()
            gone_deadline = time.monotonic() + min(10.0, max(1.0, deadline - time.monotonic()))
            while os.path.exists(args.port) and time.monotonic() < gone_deadline:
                time.sleep(0.1)
            if args.verbose:
                print("waiting for Xous USB reconnect", file=sys.stderr, flush=True)
        except Exception:
            port.close()
            raise

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
