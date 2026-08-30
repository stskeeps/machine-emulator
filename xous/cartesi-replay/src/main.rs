use sha2::{Digest, Sha256};
use std::ffi::{CStr, c_char, c_ulong, c_ulonglong};
#[cfg(target_os = "xous")]
use std::time::Duration;
use tiny_keccak::{Hasher, Keccak};

type MachineHash = [u8; 32];
#[cfg(target_os = "xous")]
const LENGTH_BYTES: usize = 8;
const JOURNAL_BYTES: usize = 96;
#[cfg(target_os = "xous")]
const READY_MESSAGE: &[u8] = b"CARTESI_READY\n";
#[cfg(target_os = "xous")]
const RECEIVED_MESSAGE: &[u8] = b"CARTESI_RECEIVED\n";
#[cfg(target_os = "xous")]
const JOURNAL_MESSAGE: &[u8] = b"CARTESI_JOURNAL\n";

extern "C" {
    fn risc0_replay_steps(
        raw_log_data: *mut u8,
        raw_log_size: c_ulonglong,
        out_root_hash_before: *mut u8,
        out_mcycle_count: *mut u64,
        out_root_hash_after: *mut u8,
    );
}

#[no_mangle]
pub extern "C" fn zk_abort_with_msg(msg: *const c_char) -> ! {
    let msg = unsafe { CStr::from_ptr(msg) }.to_string_lossy();
    panic!("Cartesi replay failed: {msg}");
}

#[no_mangle]
pub extern "C" fn zk_putchar(_c: u8) {}

// The replay core does not allocate C++ objects dynamically. These two ABI
// shims replace the no-op definitions in RISC Zero's freestanding runtime
// without importing that runtime (whose `abort` conflicts with Xous std).
#[export_name = "_ZdlPvj"]
pub extern "C" fn cxx_sized_delete(_pointer: *mut u8, _size: usize) {}

#[no_mangle]
pub extern "C" fn atexit(_callback: unsafe extern "C" fn()) -> i32 {
    0
}

fn hash_bytes(kind: u64, data: &[u8]) -> MachineHash {
    match kind {
        0 => {
            let mut output = [0; 32];
            let mut hasher = Keccak::v256();
            hasher.update(data);
            hasher.finalize(&mut output);
            output
        }
        1 => Sha256::digest(data).into(),
        _ => zk_abort_with_msg(c"unsupported hash function".as_ptr()),
    }
}

fn merkle_tree_hash(kind: u64, data: &[u8]) -> MachineHash {
    if data.len() <= 32 {
        return hash_bytes(kind, data);
    }
    if !data.len().is_power_of_two() {
        zk_abort_with_msg(c"Merkle input size is not a power of two".as_ptr());
    }
    let (left, right) = data.split_at(data.len() / 2);
    let mut pair = [0; 64];
    pair[..32].copy_from_slice(&merkle_tree_hash(kind, left));
    pair[32..].copy_from_slice(&merkle_tree_hash(kind, right));
    hash_bytes(kind, &pair)
}

#[no_mangle]
pub unsafe extern "C" fn zk_merkle_tree_hash(
    kind: u64,
    data: *const c_char,
    size: c_ulong,
    output: *mut c_char,
) {
    let data = std::slice::from_raw_parts(data.cast::<u8>(), size as usize);
    let hash = merkle_tree_hash(kind, data);
    output.cast::<u8>().copy_from_nonoverlapping(hash.as_ptr(), hash.len());
}

#[no_mangle]
pub unsafe extern "C" fn zk_concat_hash(
    kind: u64,
    left: *const c_char,
    right: *const c_char,
    output: *mut c_char,
) {
    let mut pair = [0; 64];
    pair[..32].copy_from_slice(std::slice::from_raw_parts(left.cast::<u8>(), 32));
    pair[32..].copy_from_slice(std::slice::from_raw_parts(right.cast::<u8>(), 32));
    let hash = hash_bytes(kind, &pair);
    output.cast::<u8>().copy_from_nonoverlapping(hash.as_ptr(), hash.len());
}

#[cfg(target_os = "xous")]
fn receive_log(usb: &usb_bao1x::UsbHid) -> Vec<u8> {
    let mut input = Vec::new();
    log::info!("CARTESI_RX_WAIT");
    let expected = loop {
        let chunk = usb.serial_wait_binary();
        log::info!("CARTESI_RX_CHUNK len={} total={}", chunk.len(), input.len() + chunk.len());
        input.extend_from_slice(&chunk);
        if input.len() >= LENGTH_BYTES {
            let length = u64::from_le_bytes(input[..LENGTH_BYTES].try_into().unwrap());
            // The USB IPC hook can produce an empty zero-filled response while the
            // listener is being installed. Ignore that response and keep waiting.
            if length == 0 {
                input.clear();
                continue;
            }
            let expected = usize::try_from(length).expect("step log is too large for this target");
            log::info!("CARTESI_RX_LENGTH expected={}", expected);
            break expected;
        }
    };
    assert!(expected > 0, "empty step log");
    assert!(expected <= isize::MAX as usize, "step log is too large");
    while input.len() - LENGTH_BYTES < expected {
        let chunk = usb.serial_wait_binary();
        log::info!("CARTESI_RX_CHUNK len={} total={}", chunk.len(), input.len() + chunk.len());
        input.extend_from_slice(&chunk);
    }
    log::info!("CARTESI_RX_COMPLETE bytes={}", expected);
    assert_eq!(input.len() - LENGTH_BYTES, expected, "extra bytes after framed step log");
    input.drain(..LENGTH_BYTES);
    input
}

#[cfg(target_os = "xous")]
fn send_all(usb: &usb_bao1x::UsbHid, mut data: &[u8]) {
    while !data.is_empty() {
        match usb.serial_send(data) {
            Ok(0) | Err(_) => std::thread::sleep(Duration::from_millis(10)),
            Ok(sent) => data = &data[sent..],
        }
    }
    usb.serial_flush().ok();
}

fn replay(mut log: Vec<u8>) -> [u8; JOURNAL_BYTES] {
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

    let mut journal = [0; JOURNAL_BYTES];
    journal[..32].copy_from_slice(&root_before);
    journal[56..64].copy_from_slice(&mcycle.to_be_bytes());
    journal[64..].copy_from_slice(&root_after);
    journal
}

#[cfg(target_os = "xous")]
fn main() -> ! {
    log_server::init_wait().expect("could not connect to the Xous log server");
    let usb = usb_bao1x::UsbHid::new();
    // Let usb-bao1x finish registering with xous-log before installing the mirror hook.
    std::thread::sleep(Duration::from_millis(500));
    usb.serial_console_input_injection();
    // Install the Enter handshake immediately so the host cannot race past it.
    loop {
        let input = usb.serial_wait_ascii(None);
        if input.contains('\n') || input.contains('\r') {
            break;
        }
    }
    send_all(&usb, READY_MESSAGE);
    let log = receive_log(&usb);
    send_all(&usb, RECEIVED_MESSAGE);
    let journal = replay(log);
    send_all(&usb, JOURNAL_MESSAGE);
    send_all(&usb, &journal);
    usb.serial_clear_input_hooks();
    xous::terminate_process(0)
}

#[cfg(not(target_os = "xous"))]
fn main() -> ! {
    let input = std::env::var("CARTESI_LOG").expect("set CARTESI_LOG to a step-log path");
    let output = std::env::var("CARTESI_JOURNAL").expect("set CARTESI_JOURNAL to an output path");
    let log = std::fs::read(input).expect("could not read CARTESI_LOG");
    assert!(!log.is_empty(), "empty step log");
    std::fs::write(output, replay(log)).expect("could not write CARTESI_JOURNAL");
    xous::terminate_process(0)
}
