// Copyright Cartesi and individual authors (see AUTHORS)
// SPDX-License-Identifier: LGPL-3.0-or-later

// Host/Xous-neutral entry point for the replay core. In particular, this does
// not include risc0/cpp/zk-runtime.hpp, whose printf substitutions are intended
// only for RISC Zero's freestanding C++ environment.

#include "machine-hash.hpp"
#include "interpret.hpp"
#include "replay-step-state-access.hpp"
#include <cstdint>
#include <cstring>

using namespace cartesi;

extern "C" void risc0_replay_steps(
    unsigned char *step_log_image,
    uint64_t step_log_image_size,
    unsigned char *out_root_hash_before,
    uint64_t *out_mcycle_count,
    unsigned char *out_root_hash_after) {
    replay_step_state_access::context context{};
    replay_step_state_access access(context, step_log_image, step_log_image_size);
    uint64_t mcycle_end{};
    (void) __builtin_add_overflow(access.read_mcycle(), context.logged_mcycle_count, &mcycle_end);
    interpret<replay_step_state_access &>(access, mcycle_end);
    access.finish();
    std::memcpy(out_root_hash_before, context.logged_root_hash_before.data(), 32);
    *out_mcycle_count = context.logged_mcycle_count;
    std::memcpy(out_root_hash_after, context.logged_root_hash_after.data(), 32);
}
