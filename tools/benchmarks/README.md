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
  tests/build/images tests/build/machine/first-cycle
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
