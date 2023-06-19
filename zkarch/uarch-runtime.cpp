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

#include "uarch-runtime.h"
#include "tty.h"
#include "uarch-constants.h"
#include <algorithm>

using namespace cartesi;

extern "C" void __cxa_pure_virtual() {
    abort();
}

void operator delete(void *, unsigned long) {}
void operator delete(void*, unsigned int) {}
extern "C" void __assert_func(const char *file, int line, const char *, const char *e) {}

extern "C" void __assert_fail(const char *__assertion, const char *__file, unsigned int __line,
    const char *__function) {}

extern "C" void *memmove(void *dest, const void *src, size_t n) {
    if (!n || src == dest) {
        return dest;
    }
    auto s = const_cast<char *>(reinterpret_cast<const char *>(src));
    auto d = reinterpret_cast<char *>(dest);
    if (d < s) {
        for (; n; n--)
            *d++ = *s++;
    } else {
        while (n--)
            d[n] = s[n];
    }
    return dest;
}

#if 0
extern "C" void *memcpy(void *dest, const void *src, size_t n) {
    auto s = reinterpret_cast<const unsigned char *>(src);
    auto d = reinterpret_cast<unsigned char *>(dest);
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

extern "C" void *memset(void *ptr, int value, size_t num) {
    volatile unsigned char *p = reinterpret_cast<unsigned char *>(ptr);
    while (num--) {
        *p++ = value;
    }
    return ptr;
}
#endif

extern "C" int __ucmpdi2 (long long x, long long y) { return x < y ? 0 : x == y ? 1 : 2; }

extern "C" int __clzsi2 (int val);
extern "C" int __clzdi2 (long long val)
{
  if (val >> 32)
    {
      return __clzsi2 (val >> 32);
    }
  else
    {
      return __clzsi2 (val) + 32;
    }
}


extern "C" void _putchar(char character);
extern "C" [[noreturn]] void abort(void);

namespace cartesi {

void tty_initialize(void) {}

void tty_finalize(void) {}

void tty_poll_console(uint64_t wait) {
    (void) wait;
}

int tty_getchar(void) {
    return 0;
}

void tty_putchar(uint8_t ch) {
    _putchar(ch);
}

} // namespace cartesi
