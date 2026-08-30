# Cartesi replay for Baochip (out of tree)

This standalone Xous application replays one Cartesi Machine step log without
RISC Zero and returns the same 96-byte journal as the RISC0 guest. It is kept
outside `xous-core` and pins its Xous dependency to upstream commit
`afa89859512d17bc7a01e192d471cb6678f067ba` (`v0.10.2`). The loader and kernel
installed on the device must come from that same Xous revision.

The containing `machine-emulator` checkout supplies the freestanding replay
object. A standalone sibling checkout is also detected automatically. Override
its location with `MACHINE_EMULATOR_DIR`, or point directly at the object with
`CARTESI_REPLAY_OBJ`.

## Build

Build the RV32 replay object:

```sh
make -C ../../risc0/cpp baochip-replay-steps.o
```

Then build this standalone Xous app for Dabao:

```sh
cargo build --release --target riscv32imac-unknown-xous-elf \
  --features board-dabao --features bao1x
```

The included `build.rs` installs the version-matched Xous Rust toolkit when it
is missing. Convert the resulting ELF into a detached application image:

```sh
cargo install xous-tools
xous-app-uf2 --elf target/riscv32imac-unknown-xous-elf/release/cartesi-replay
```

This creates `apps.uf2`. The device must already have the matching Dabao
loader and kernel, which can be built from `xous-core` at the pinned revision:

```sh
cargo xtask dabao dabao-console --no-timestamp
```

Flash `loader.uf2` and `xous.uf2` from that build once, then flash this
repository's `apps.uf2` whenever the replay app changes.

## Protocol

Send an unsigned 64-bit little-endian log length immediately followed by that
many log bytes over Baochip USB CDC. After the host opens the port, send an ASCII
Enter (`\n`). The app then emits the exact banner `CARTESI_READY\n`; wait for
that banner before sending the binary frame. The app replies with exactly 96 bytes:

```text
root_hash_before[32] || zero_padding[24] || mcycle_be[8] || root_hash_after[32]
```

Use the included helper:

```sh
python3 send-log.py /dev/ttyACM0 step.log journal.bin
```

The replay app owns the binary serial hooks while it runs; do not send a log
through an interactive terminal program at the same time.
