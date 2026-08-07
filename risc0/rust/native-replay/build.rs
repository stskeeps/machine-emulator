use std::{env, path::PathBuf};

fn main() {
    let manifest = PathBuf::from(env::var_os("CARGO_MANIFEST_DIR").unwrap());
    let emulator = manifest.join("../../..");
    let src = emulator.join("src");
    let risc0_cpp = emulator.join("risc0/cpp");

    for path in [
        manifest.join("src/replay-steps.cpp"),
        src.join("interpret.cpp"),
        src.join("htif-address-range.cpp"),
        src.join("plic-address-range.cpp"),
        src.join("clint-address-range.cpp"),
        src.join("cm-version.h"),
        src.join("interpret-jump-table.hpp"),
    ] {
        println!("cargo:rerun-if-changed={}", path.display());
    }

    cc::Build::new()
        .cpp(true)
        .std("c++23")
        .include(&src)
        .include(emulator.join("third-party"))
        .include(emulator.join("third-party/llvm-flang-uint128"))
        .include(&risc0_cpp)
        .define("ZKARCHITECTURE", "1")
        .define("NO_STD_VECTOR", "1")
        .define("AVOID_NATIVE_UINT128_T", "1")
        .flag_if_supported("-fno-exceptions")
        .flag_if_supported("-fno-rtti")
        .flag_if_supported("-fno-strict-aliasing")
        .flag_if_supported("-fno-strict-overflow")
        .flag_if_supported("-fno-delete-null-pointer-checks")
        .file(manifest.join("src/replay-steps.cpp"))
        .file(src.join("interpret.cpp"))
        .file(src.join("htif-address-range.cpp"))
        .file(src.join("plic-address-range.cpp"))
        .file(src.join("clint-address-range.cpp"))
        .compile("cartesi_replay_steps");
}
