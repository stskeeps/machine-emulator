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

/// \file
/// \brief Physical memory attributes implementation

#include <fcntl.h>    // open
#include <sys/mman.h> // mmap, munmap
#include <sys/stat.h> // fstat
#include <unistd.h>   // close

#include <cerrno>
#include <cstring>
#include <string>
#include <system_error>

#include "pma.h"
#include "unique-c-ptr.h"

namespace cartesi {

using namespace std::string_literals;

void pma_memory::release(void) {
    if (m_backing_file >= 0) {
        munmap(m_host_memory, m_length);
        close(m_backing_file);
        m_backing_file = -1;
    } else {
        std::free(m_host_memory); // NOLINT(cppcoreguidelines-no-malloc)
    }
    m_host_memory = nullptr;
    m_length = 0;
}

pma_memory::~pma_memory() {
    release();
}

pma_memory::pma_memory(pma_memory &&other) noexcept :
    m_length{std::move(other.m_length)},
    m_host_memory{std::move(other.m_host_memory)},
    m_backing_file{std::move(other.m_backing_file)} {
    // set other to safe state
    other.m_host_memory = nullptr;
    other.m_backing_file = -1;
    other.m_length = 0;
}

pma_memory::pma_memory(const std::string &description, uint64_t length, const callocd &c) :
    m_length{length},
    m_host_memory{nullptr},
    m_backing_file{-1} {
    (void) c;
    // use calloc to improve performance
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-prefer-member-initializer)
    m_host_memory = static_cast<unsigned char *>(std::calloc(1, length));
    if (!m_host_memory) {
        throw std::runtime_error{"error allocating memory for "s + description};
    }
}

pma_memory::pma_memory(const std::string &description, uint64_t length, const mockd &m) :
    m_length{length},
    m_host_memory{nullptr},
    m_backing_file{-1} {
    (void) m;
    (void) description;
}

pma_memory::pma_memory(const std::string &description, uint64_t length, const std::string &path, const callocd &c) :
    pma_memory{description, length, c} {
    // Try to load image file, if any
    if (!path.empty()) {
        auto fp = unique_fopen(path.c_str(), "rb", std::nothrow_t{});
        if (!fp) {
            throw std::system_error{errno, std::generic_category(),
                "error opening image file '"s + path + "' when initializing "s + description};
        }
        // Get file size
        if (fseek(fp.get(), 0, SEEK_END)) {
            throw std::system_error{errno, std::generic_category(),
                "error obtaining length of image file '"s + path + "' when initializing "s + description};
        }
        auto file_length = ftell(fp.get());
        if (fseek(fp.get(), 0, SEEK_SET)) {
            throw std::system_error{errno, std::generic_category(),
                "error obtaining length of image file '"s + path + "' when initializing "s + description};
        }
        // Check against PMA range size
        if (static_cast<uint64_t>(file_length) > length) {
            throw std::runtime_error{"image file '"s + path + "' of "s + description + " is too large for range"s};
        }
        // Read to host memory
        auto read = fread(m_host_memory, 1, length, fp.get());
        (void) read;
        if (ferror(fp.get())) {
            throw std::system_error{errno, std::generic_category(),
                "error reading from image file '"s + path + "' when initializing "s + description};
        }
    }
}

pma_memory::pma_memory(const std::string &description, uint64_t length, const std::string &path, const mmapd &m) :
    m_length{length},
    m_host_memory{nullptr},
    m_backing_file{-1} {
    if (path.empty()) {
        throw std::runtime_error{"image file must be specified for "s + description};
    }

    int oflag = m.shared ? O_RDWR : O_RDONLY;
    int mflag = m.shared ? MAP_SHARED : MAP_PRIVATE;

    // Try to open image file
    int backing_file = open(path.c_str(), oflag);
    if (backing_file < 0) {
        throw std::system_error{errno, std::generic_category(),
            "could not open image file '"s + path + "' when initializing "s + description};
    }

    // Try to get file size
    struct stat statbuf {};
    if (fstat(backing_file, &statbuf) < 0) {
        close(backing_file);
        throw std::system_error{errno, std::generic_category(),
            "unable to obtain length of image file '"s + path + "' when initializing "s + description};
    }

    // Check that it matches range length
    if (static_cast<uint64_t>(statbuf.st_size) != length) {
        close(backing_file);
        throw std::invalid_argument{"image file '"s + path + "' size ("s +
            std::to_string(static_cast<uint64_t>(statbuf.st_size)) + ") does not match range length ("s +
            std::to_string(length) + ") of "s + description};
    }

    // Try to map image file to host memory
    auto *host_memory =
        static_cast<unsigned char *>(mmap(nullptr, length, PROT_READ | PROT_WRITE, mflag, backing_file, 0));
    if (host_memory == MAP_FAILED) { // NOLINT(cppcoreguidelines-pro-type-cstyle-cast,performance-no-int-to-ptr)
        close(backing_file);
        throw std::system_error{errno, std::generic_category(),
            "could not map image file '"s + path + "' to memory when initializing "s + description};
    }

    // Finally store everything in object
    m_host_memory = host_memory;
    m_backing_file = backing_file;
}

pma_memory &pma_memory::operator=(pma_memory &&other) noexcept {
    release();
    // copy from other
    m_host_memory = std::move(other.m_host_memory);
    m_backing_file = std::move(other.m_backing_file);
    m_length = std::move(other.m_length);
    // set other to safe state
    other.m_host_memory = nullptr;
    other.m_backing_file = -1;
    other.m_length = 0;
    return *this;
}

uint64_t pma_entry::get_istart(void) const {
    uint64_t istart = m_start;
    istart |= (static_cast<uint64_t>(get_istart_M()) << PMA_ISTART_M_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_IO()) << PMA_ISTART_IO_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_E()) << PMA_ISTART_E_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_R()) << PMA_ISTART_R_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_W()) << PMA_ISTART_W_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_X()) << PMA_ISTART_X_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_IR()) << PMA_ISTART_IR_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_IW()) << PMA_ISTART_IW_SHIFT);
    istart |= (static_cast<uint64_t>(get_istart_DID()) << PMA_ISTART_DID_SHIFT);
    return istart;
}

uint64_t pma_entry::get_ilength(void) const {
    return m_length;
}

bool pma_peek_error(const pma_entry &, const machine &, uint64_t, const unsigned char **, unsigned char *) {
    return false;
}

/// \brief Memory range peek callback. See pma_peek.
static bool memory_peek(const pma_entry &pma, const machine &m, uint64_t page_address, const unsigned char **page_data,
    unsigned char *scratch) {
    (void) m;
    // If page_address is not aligned, or if it is out of range, return error
    if ((page_address & (PMA_PAGE_SIZE - 1)) != 0 || page_address > pma.get_length()) {
        *page_data = nullptr;
        return false;
    }
    // If page is only partially inside range, copy to scratch
    if (page_address + PMA_PAGE_SIZE > pma.get_length()) {
        memset(scratch, 0, PMA_PAGE_SIZE);
        memcpy(scratch, pma.get_memory().get_host_memory() + page_address, pma.get_length() - page_address);
        *page_data = scratch;
        return true;
        // Otherwise, return pointer direclty into host memory
    } else {
        *page_data = pma.get_memory().get_host_memory() + page_address;
        return true;
    }
}

pma_entry make_mmapd_memory_pma_entry(const std::string &description, uint64_t start, uint64_t length,
    const std::string &path, bool shared) {
    if (length == 0) {
        throw std::invalid_argument{description + " length cannot be zero"s};
    }
    return pma_entry{description, start, length, pma_memory{description, length, path, pma_memory::mmapd{shared}},
        memory_peek};
}

pma_entry make_callocd_memory_pma_entry(const std::string &description, uint64_t start, uint64_t length) {
    if (length == 0) {
        throw std::invalid_argument{description + " length cannot be zero"s};
    }
    return pma_entry{description, start, length, pma_memory{description, length, pma_memory::callocd{}}, memory_peek};
}

pma_entry make_callocd_memory_pma_entry(const std::string &description, uint64_t start, uint64_t length,
    const std::string &path) {
    if (length == 0) {
        throw std::invalid_argument{description + " length cannot be zero"s};
    }
    return pma_entry{description, start, length, pma_memory{description, length, path, pma_memory::callocd{}},
        memory_peek};
}

pma_entry make_mockd_memory_pma_entry(const std::string &description, uint64_t start, uint64_t length) {
    if (length == 0) {
        throw std::invalid_argument{description + " length cannot be zero"s};
    }
    return pma_entry{description, start, length, pma_memory{description, length, pma_memory::mockd{}}, pma_peek_error};
}

pma_entry make_device_pma_entry(const std::string &description, uint64_t start, uint64_t length, pma_peek peek,
    const pma_driver *driver, void *context) {
    if (length == 0) {
        throw std::invalid_argument{description + " length cannot be zero"s};
    }
    return pma_entry{description, start, length, pma_device{description, driver, context}, peek};
}

pma_entry make_empty_pma_entry(const std::string &description, uint64_t start, uint64_t length) {
    return pma_entry{description, start, length};
}

} // namespace cartesi
