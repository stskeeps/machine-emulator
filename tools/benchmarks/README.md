# Machine load to first cycle benchmark

Build the benchmark from the repository root:

```sh
make -C tools/benchmarks
```

Run it against a stored machine directory:

```sh
tools/benchmarks/build/cartesi-machine-load-first-cycle \
  /path/to/stored-machine [iterations=10] [warmups=0] [sharing=none|config|all]
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
