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

#include <utility>

#include "uarch-machine.h"

namespace cartesi {

using namespace std::string_literals;

const pma_entry::flags ram_flags{
    true,                  // R
    true,                  // W
    true,                  // X
    true,                  // IR
    true,                  // IW
    PMA_ISTART_DID::memory // DID
};

uarch_machine::uarch_machine(uarch_config c) : m_s{}, m_c{std::move(c)} {
    load_config(m_c);
}

/// \brief Resets the value of halt flag
void uarch_machine::reset_state(void) {
    if (!m_s.halt_flag) {
        throw std::runtime_error("reset uarch state is not allowed when uarch is not halted");
    }
    load_config(m_c);
}

void uarch_machine::load_config(uarch_config &c) {
    m_s.pc = c.processor.pc;
    m_s.cycle = c.processor.cycle;
    m_s.halt_flag = c.processor.halt_flag;
    // General purpose registers
    for (int i = 1; i < UARCH_X_REG_COUNT; i++) {
        m_s.x[i] = c.processor.x[i];
    }
    // Register RAM pma
    if (!c.ram.image_filename.empty()) {
        m_s.ram = make_callocd_memory_pma_entry("uarch RAM", PMA_UARCH_RAM_START, c.ram.length, c.ram.image_filename)
                      .set_flags(ram_flags);
    } else if (c.ram.length > 0) {
        m_s.ram = make_callocd_memory_pma_entry("uarch RAM", PMA_UARCH_RAM_START, c.ram.length).set_flags(ram_flags);
    }
}

uint64_t uarch_machine::read_cycle(void) const {
    return m_s.cycle;
}

void uarch_machine::write_cycle(uint64_t val) {
    m_s.cycle = val;
}

uint64_t uarch_machine::read_pc(void) const {
    return m_s.pc;
}

/// \brief Gets the value of halt flag
bool uarch_machine::read_halt_flag(void) const {
    return m_s.halt_flag;
}

/// \brief Sets the value of halt flag
void uarch_machine::set_halt_flag(void) {
    m_s.halt_flag = true;
}

void uarch_machine::write_pc(uint64_t val) {
    m_s.pc = val;
}

uint64_t uarch_machine::read_x(int i) const {
    return m_s.x[i];
}

void uarch_machine::write_x(int i, uint64_t val) {
    if (i > 0) {
        m_s.x[i] = val;
    }
}

uint64_t uarch_machine::read_ram_length(void) const {
    return m_s.ram.get_length();
}

pma_entry &uarch_machine::find_pma_entry(uint64_t paddr, size_t length) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast): remove const to reuse code
    return const_cast<pma_entry &>(std::as_const(*this).find_pma_entry(paddr, length));
}

const pma_entry &uarch_machine::find_pma_entry(uint64_t paddr, size_t length) const {
    if (m_s.ram.contains(paddr, length)) {
        return m_s.ram;
    }
    return m_s.empty_pma;
}

} // namespace cartesi
