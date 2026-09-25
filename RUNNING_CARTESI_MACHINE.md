# Running a Cartesi machine and storing a snapshot

This guide uses the Linux kernel and root filesystem from Machine Emulator's
test suite. It creates a 512 MiB machine, runs it to a chosen machine-cycle
(`mcycle`) count, and stores the resulting state as a reusable snapshot.

The examples below use `2,000,000` cycles; change the target and output paths
as needed. Build and run the machine inside Machine Emulator's Docker toolchain
container so the required compiler, Lua, and libraries are available.

## Where to run the commands

On the host, first `cd` to the root of your Machine Emulator checkout—the
directory containing `Makefile`, `Dockerfile`, `src/`, `tests/`, and
`tools/`. For example, if the checkout is at `/path/to/machine-emulator`:

```sh
cd /path/to/machine-emulator
make toolchain-env
```

This target creates the `cartesi/machine-emulator:toolchain` image if needed,
mounts the host checkout at `/opt/cartesi/machine-emulator` in the container,
and opens a shell with that directory as its working directory. Run all
remaining commands in that shell, from the repository root. Thus, for the
commands below, `tests/build/images` and `tests/build/machine` are paths inside
the container and correspond to
`/path/to/machine-emulator/tests/build/images` and
`/path/to/machine-emulator/tests/build/machine` on the host. Generated assets
and snapshots remain in the host checkout because it is mounted into Docker.

The source image files are fetched into `tests/build/images`; the initial
machine and snapshot are created under `tests/build/machine`. These directories
are created as needed by the build/scripts.

## Build the test assets and Lua binding (inside the container)

```sh
make -C tests images
make -C src cartesi.so
```

`make -C tests images` fetches the test kernel and root filesystem into
`tests/build/images`. The Lua binding is built as `src/cartesi.so`.

## Create a 512 MiB initial machine

```sh
LUA_CPATH="$PWD/src/?.so;;" lua5.4 \
  tools/benchmarks/create-test-machine.lua \
  tests/build/images tests/build/machine/cartesi-512mib 512
```

The final argument is RAM size in MiB. The fixture uses the test kernel,
`rootfs.ext2`, and the `rollup-init echo-dapp --reject=0 --verbose`
entrypoint. The script stores the initial machine under
`tests/build/machine/cartesi-512mib`.

## Run it and save a snapshot at a target mcycle

```sh
LUA_CPATH="$PWD/src/?.so;;" lua5.4 \
  tools/benchmarks/snapshot-at-mcycle.lua \
  tests/build/machine/cartesi-512mib \
  tests/build/machine/cartesi-at-mcycle-2000000 \
  2000000
```

The snapshot script loads the stored machine, reads its current `mcycle`, runs
until it reaches at least the target, then stores the resulting state at the
output path. It reports the actual cycle count when it finishes. The original
initial machine remains unchanged.

To continue from the snapshot, use its directory as the source for another
snapshot run, or load it in a Machine Emulator program. Keep each output path
new: storing a machine does not overwrite an existing stored-machine
directory.

## Run with visible console and SRET/WFI tracing

The trace runner loads a stored machine without changing the stored source,
shows guest console output on stdout, and writes one trace line to stderr each
time the guest executes a valid SRET or WFI instruction. Pass an optional
`max-mcycle` to bound the run; otherwise it runs until halt, manual yield, or
machine-cycle overflow.

```sh
LUA_PATH="$PWD/src/?.lua;$PWD/tests/lua/?.lua;;" \
LUA_CPATH="$PWD/src/?.so;;" \
lua5.4 tools/benchmarks/trace-sret-wfi.lua \
  tests/build/machine/cartesi-at-mcycle-2000000
```

SRET/WFI breaks are enabled for this runner only. The guest console remains on
stdout; `TRACE SRET` and `TRACE WFI` records go to stderr so they do not corrupt
the guest's console stream.

## NVRAM configuration and access

NVRAM is an optional raw-memory region, configured as an entry in the machine's
`nvram` array. It has no filesystem layer. For example, to add a 4 KiB region
with a user-facing label and initial bytes from a file:

```lua
nvram = {
    {
        label = "cartesi-input",
        length = 4096,
        backing_store = { data_filename = "input.raw" },
    },
}
```

`start` can optionally specify the guest physical address; otherwise Machine
Emulator places NVRAM after RAM and configured flash drives. Each region's
`backing_store.shared` controls whether guest writes are reflected in its
backing file while the machine runs. With the default `false`, writes are part
of the in-memory machine state and can be captured by storing the machine.
NVRAM ranges are writable by default; `read_only = true` prevents both host and
guest writes. The backing-store options `create` and `truncate` are available
for file-backed NVRAM as well (both require `shared = true`); lengths must be a
multiple of 4 KiB.

The guest sees NVRAM through Linux UIO as `/dev/uioN` (for example, use
`mmap`; it is not a normal `read()`/`write()` device). The label is published
in the device tree and can be resolved with the guest `nvram` utility. Guest
access is root-owned by default; set the NVRAM `user` option if the unprivileged
`dapp` process needs access. Machine Emulator's test rootfs includes
`readmmap`/`writemmap` utilities for convenient guest-side data transfer.

Each NVRAM is exposed as a `generic-uio` node with its own PLIC interrupt
source. Source IDs are assigned after the configured virtio devices, in NVRAM
configuration order, and are 1-based. The PLIC supports source IDs 1–31, so the
combined number of virtio devices and NVRAMs cannot exceed 31.

This wires the interrupt into Linux's UIO device, but does not make ordinary
NVRAM reads or writes raise it. The host/device model must assert the assigned
PLIC source when it has an event to report; userspace can then wait on the UIO
device's `/dev/uioN` descriptor. The machine API exposes `plic_girqpend` and
`mip` registers if a host controller needs to inject a pending source directly.

There are no NVRAM-specific host read/write calls: use the generic machine
memory API at the NVRAM's guest physical address. In Lua, find the labeled
range in the initial config and use `machine:read_memory(start, length)` or
`machine:write_memory(start, data)`. In C, the corresponding calls are
`cm_read_memory()` and `cm_write_memory()` with the physical address. For
example:

```lua
local config = machine:get_initial_config()
local input = cartesi.util.find_drive(config, "nvram", "cartesi-input")
local bytes = machine:read_memory(input.start, 4096)
```

To supply different input after creating/loading a machine, use
`machine:replace_memory_range()` with an NVRAM config that identifies the
existing region and points its backing store at the new input file.

## Notes

- The scripts and output directories are under `tools/benchmarks` and
  `tests/build`, respectively; generated test assets are not source files.
- If Lua cannot find the `cartesi` module, confirm the command is run from the
  repository root and that `LUA_CPATH` includes `src/?.so`.
- Snapshot advancement time depends on the target cycle and host. The snapshot
  is a Cartesi machine state, not a host process checkpoint.
