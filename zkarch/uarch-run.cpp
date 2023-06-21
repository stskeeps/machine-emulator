// Copyright 2019 Cartesi Pte. Ltd.
//
// This file is part of the machine-emulator. The machine-emulator is free
// software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// The machine-emulator is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the machine-emulator. If not, see http://www.gnu.org/licenses/.
//

#define MICROARCHITECTURE 1

#include "uarch-runtime.h" // must be included first, because of assert

#include "interpret.h"
#include "uarch-machine-state-access.h"
#include <cinttypes>

using namespace cartesi;

extern "C" uint64_t run_uarch(uint64_t mcycle_begin, uint64_t mcycle_end) {
#ifdef ZKARCH_DEBUG
    printf("hello from uarch\n");
#endif
    uintptr_t pma_shadow_state = page_in(
        (uint64_t) PMA_SHADOW_STATE_START);
    uintptr_t pma_shadow_pmas = page_in(
        (uint64_t) PMA_SHADOW_PMAS_START_DEF);

    uintptr_t pma_shadow_tlb = page_in_with_length((uint64_t) PMA_SHADOW_TLB_START_DEF, PMA_SHADOW_TLB_LENGTH_DEF);
        
    uarch_machine_state_access a(pma_shadow_state, pma_shadow_pmas, pma_shadow_tlb);

    uint64_t mcycle = a.read_mcycle();
    if (mcycle_begin != mcycle) {
        abort();
    }
   
#ifdef ZKARCH_DEBUG
    printf("mcycle now: %llu\n", mcycle);
#endif
    // We want to advance the cartesi machine to the next mcycle
    for (;;) {
        if (a.read_iflags_H() || a.read_iflags_Y()) {
            mcycle = a.read_mcycle();
            break;
        }
        interpret(a, mcycle_end);
        mcycle = a.read_mcycle();
        if (mcycle >= mcycle_end) {
            break;
        }
    }
    page_dirty((uint64_t) PMA_SHADOW_STATE_START);
    page_dirty((uint64_t) PMA_SHADOW_TLB_START_DEF);
    page_dirty((uint64_t) PMA_SHADOW_PMAS_START_DEF);
    a.do_dirty_tlb();

    return mcycle;
}
