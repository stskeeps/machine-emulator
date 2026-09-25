# Machine load to first cycle benchmark

Fetch the Linux kernel and root filesystem used by the emulator tests, then build
the benchmark and Lua binding from the repository root:

```sh
make -C tests images
make -C tools/benchmarks
make -C src cartesi.so
```

Create an initial-state stored machine using the same 64 MiB RAM, kernel,
rootfs, and entrypoint configuration as `tests/lua/create-machines.lua`:

```sh
LUA_CPATH="$PWD/src/?.so;;" lua5.4 \
  tools/benchmarks/create-test-machine.lua \
  tests/build/images tests/build/machine/first-cycle 64
```

The last argument sets RAM size in MiB; use `512` to generate a 512 MiB fixture.

To prepare a snapshot at a later mcycle (for example, 2,000,000), advance and
store a copy, then point the benchmark at that copy:

```sh
LUA_CPATH="$PWD/src/?.so;;" lua5.4 \
  tools/benchmarks/snapshot-at-mcycle.lua \
  tests/build/machine/first-cycle-512mib \
  tests/build/machine/at-mcycle-2000000 2000000

tools/benchmarks/build/cartesi-machine-load-first-cycle \
  tests/build/machine/at-mcycle-2000000 10 0 none
```

Then run the benchmark:

```sh
tools/benchmarks/build/cartesi-machine-load-first-cycle \
  tests/build/machine/first-cycle 10 0 none
```

Each measured iteration loads the stored machine, reads its initial `mcycle`,
runs until `mcycle` advances by one, then destroys and deletes the machine. The
CSV output separates load latency, run-to-first-cycle latency, elapsed time from
before load through the first cycle, and destroy latency. It then reports mean,
median, 95th percentile, minimum, and maximum for each phase. The default keeps
the first iteration in the results so the coldest observed load is visible;
warmups can be specified to exclude initial iterations.

`sharing=none` is the default. Choose `config` or `all` to benchmark the
corresponding backing-store sharing mode. Reusing the same directory makes
filesystem cache warming part of repeated measurements; use `warmups=0` and
inspect per-iteration rows when comparing first-load behavior.

## Trace SRET and WFI while showing the guest console

Load a stored machine with `trace-sret-wfi.lua` to stream guest console output
to stdout and print each valid SRET/WFI break (with mcycle, PC, and privilege)
to stderr. Both break options are enabled only in the runner's runtime config.
An optional second argument bounds execution by mcycle; otherwise it runs to a
fixed point.

```sh
LUA_PATH="$PWD/src/?.lua;$PWD/tests/lua/?.lua;;" \
LUA_CPATH="$PWD/src/?.so;;" \
lua5.4 tools/benchmarks/trace-sret-wfi.lua \
  tests/build/machine/at-mcycle-2000000 10000000
```
