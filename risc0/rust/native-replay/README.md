# Native Cartesi step-log replay

`cartesi-native-replay` runs the same C++ replay core as the RISC0 guest, but
uses the host CPU directly and emits no proof or receipt.

Build and test it from `risc0/rust`:

```
make -C ../../src cm-version.h interpret-jump-table.hpp
cargo test -p cartesi-native-replay
```

Read and write files:

```
cargo run -p cartesi-native-replay -- step.log journal.bin
```

Or use stdin/stdout:

```
cat step.log | cargo run -q -p cartesi-native-replay > journal.bin
```

The output is the same 96-byte Solidity ABI journal used by the RISC0 guest.
The input cannot be processed as a stream internally: replay needs the full log
in writable memory because it updates page data in place.
