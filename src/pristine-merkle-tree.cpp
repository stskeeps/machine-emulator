// Copyright 2021 Cartesi Pte. Ltd.
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

#include "pristine-merkle-tree.h"

#include <stdexcept>

/// \file
/// \brief Pristine Merkle tree implementation.

namespace cartesi {

pristine_merkle_tree::pristine_merkle_tree(int log2_root_size, int log2_word_size) :
    m_log2_root_size{log2_root_size},
    m_log2_word_size{log2_word_size},
    m_hashes(std::max(0, log2_root_size - log2_word_size + 1)) {
    if (log2_root_size < 0) {
        throw std::out_of_range{"log2_root_size is negative"};
    }
    if (log2_word_size < 0) {
        throw std::out_of_range{"log2_word_size is negative"};
    }
    if (log2_word_size > log2_root_size) {
        throw std::out_of_range{"log2_word_size is greater than log2_root_size"};
    }
    std::vector<uint8_t> word(1 << log2_word_size, 0);
    assert(word.size() == (UINT64_C(1) << log2_word_size));
    hasher_type h;
    h.begin();
    h.add_data(word.data(), word.size());
    h.end(m_hashes[0]);
    for (unsigned i = 1; i < m_hashes.size(); ++i) {
        get_concat_hash(h, m_hashes[i - 1], m_hashes[i - 1], m_hashes[i]);
    }
}

const pristine_merkle_tree::hash_type &pristine_merkle_tree::get_hash(int log2_size) const {
    if (log2_size < m_log2_word_size || log2_size > m_log2_root_size) {
        throw std::out_of_range{"log2_size is out of range"};
    }
    return m_hashes[log2_size - m_log2_word_size];
}

} // namespace cartesi
