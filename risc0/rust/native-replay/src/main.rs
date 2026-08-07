use sha2::{Digest, Sha256};
use std::{
    env,
    ffi::{c_char, c_ulong, c_ulonglong, CStr},
    fs::File,
    io::{self, Read, Write},
    process::ExitCode,
};
use tiny_keccak::{Hasher, Keccak};

type MachineHash = [u8; 32];

unsafe extern "C" {
    fn risc0_replay_steps(
        raw_log_data: *mut u8,
        raw_log_size: c_ulonglong,
        out_root_hash_before: *mut u8,
        out_mcycle_count: *mut u64,
        out_root_hash_after: *mut u8,
    );
}

#[unsafe(no_mangle)]
pub extern "C" fn zk_abort_with_msg(msg: *const c_char) -> ! {
    let msg = unsafe { CStr::from_ptr(msg) }.to_string_lossy();
    eprintln!("replay failed: {msg}");
    std::process::abort();
}

#[unsafe(no_mangle)]
pub extern "C" fn zk_putchar(c: u8) {
    let _ = io::stderr().write_all(&[c]);
}

fn hash_bytes(kind: u64, data: &[u8]) -> MachineHash {
    match kind {
        0 => {
            let mut result = [0; 32];
            let mut hash = Keccak::v256();
            hash.update(data);
            hash.finalize(&mut result);
            result
        }
        1 => Sha256::digest(data).into(),
        _ => zk_abort_with_msg(c"unsupported hash function".as_ptr()),
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn zk_merkle_tree_hash(
    kind: u64,
    data: *const c_char,
    size: c_ulong,
    result: *mut c_char,
) {
    let data = unsafe { std::slice::from_raw_parts(data.cast::<u8>(), size as usize) };
    let hash = if data.len() <= 32 {
        hash_bytes(kind, data)
    } else {
        if !data.len().is_power_of_two() {
            zk_abort_with_msg(c"Merkle input size is not a power of two".as_ptr());
        }
        let (left, right) = data.split_at(data.len() / 2);
        let mut pair = [0; 64];
        pair[..32].copy_from_slice(&merkle_tree_hash(kind, left));
        pair[32..].copy_from_slice(&merkle_tree_hash(kind, right));
        hash_bytes(kind, &pair)
    };
    unsafe {
        result
            .cast::<u8>()
            .copy_from_nonoverlapping(hash.as_ptr(), 32)
    };
}

fn merkle_tree_hash(kind: u64, data: &[u8]) -> MachineHash {
    let mut result = [0; 32];
    unsafe {
        zk_merkle_tree_hash(
            kind,
            data.as_ptr().cast(),
            data.len() as c_ulong,
            result.as_mut_ptr().cast(),
        )
    };
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn zk_concat_hash(
    kind: u64,
    left: *const c_char,
    right: *const c_char,
    result: *mut c_char,
) {
    let mut pair = [0; 64];
    unsafe {
        pair[..32].copy_from_slice(std::slice::from_raw_parts(left.cast::<u8>(), 32));
        pair[32..].copy_from_slice(std::slice::from_raw_parts(right.cast::<u8>(), 32));
    }
    let hash = hash_bytes(kind, &pair);
    unsafe {
        result
            .cast::<u8>()
            .copy_from_nonoverlapping(hash.as_ptr(), 32)
    };
}

fn journal(root_before: &MachineHash, mcycle: u64, root_after: &MachineHash) -> [u8; 96] {
    let mut output = [0; 96];
    output[..32].copy_from_slice(root_before);
    output[56..64].copy_from_slice(&mcycle.to_be_bytes());
    output[64..].copy_from_slice(root_after);
    output
}

fn open_input(path: Option<&str>) -> io::Result<Box<dyn Read>> {
    match path {
        Some("-") | None => Ok(Box::new(io::stdin().lock())),
        Some(path) => Ok(Box::new(File::open(path)?)),
    }
}

fn open_output(path: Option<&str>) -> io::Result<Box<dyn Write>> {
    match path {
        Some("-") | None => Ok(Box::new(io::stdout().lock())),
        Some(path) => Ok(Box::new(File::create(path)?)),
    }
}

fn run() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<_> = env::args().skip(1).collect();
    if args.len() > 2 || args.iter().any(|arg| arg == "-h" || arg == "--help") {
        eprintln!("Usage: cartesi-native-replay [LOG|-] [JOURNAL|-]");
        eprintln!("Defaults to stdin and stdout. The journal is exactly 96 binary bytes.");
        return if args.len() <= 2 {
            Ok(())
        } else {
            Err("too many arguments".into())
        };
    }

    let mut log = Vec::new();
    open_input(args.first().map(String::as_str))?.read_to_end(&mut log)?;
    if log.is_empty() {
        return Err("empty step log".into());
    }

    let mut root_before = [0; 32];
    let mut mcycle = 0;
    let mut root_after = [0; 32];
    unsafe {
        risc0_replay_steps(
            log.as_mut_ptr(),
            log.len() as c_ulonglong,
            root_before.as_mut_ptr(),
            &mut mcycle,
            root_after.as_mut_ptr(),
        );
    }

    open_output(args.get(1).map(String::as_str))?.write_all(&journal(
        &root_before,
        mcycle,
        &root_after,
    ))?;
    Ok(())
}

fn main() -> ExitCode {
    match run() {
        Ok(()) => ExitCode::SUCCESS,
        Err(error) => {
            eprintln!("cartesi-native-replay: {error}");
            ExitCode::FAILURE
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn journal_is_solidity_abi_encoding() {
        let before = [0x11; 32];
        let after = [0x22; 32];
        let encoded = journal(&before, 0x0102_0304_0506_0708, &after);
        assert_eq!(&encoded[..32], &before);
        assert_eq!(&encoded[32..56], &[0; 24]);
        assert_eq!(&encoded[56..64], &[1, 2, 3, 4, 5, 6, 7, 8]);
        assert_eq!(&encoded[64..], &after);
    }

    #[test]
    fn known_empty_hashes() {
        assert_eq!(
            hex(&hash_bytes(0, b"")),
            "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470"
        );
        assert_eq!(
            hex(&hash_bytes(1, b"")),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
        );
    }

    fn hex(bytes: &[u8]) -> String {
        bytes.iter().map(|byte| format!("{byte:02x}")).collect()
    }
}
