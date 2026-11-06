// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include "selftest.hpp"

#include <stdint.h>

#include "lib/kprintf.hpp"

void kprintf_selftest() {
    kprintf("--- kprintf selftest ---\n");

    kprintf("literal text\n");                            // literal text
    kprintf("%%\n");                                      // %
    kprintf("%c%c%c\n", 'a', 'b', 'c');                   // abc
    kprintf("%s\n", "hello");                             // hello
    kprintf("%s\n", static_cast<const char *>(nullptr));  // (null)

    kprintf("%u\n", 0u);                                // 0
    kprintf("%u\n", 1234567890u);                       // 1234567890
    kprintf("%u\n", 4294967295u);                       // 4294967295
    kprintf("%d\n", 0);                                 // 0
    kprintf("%d\n", -1);                                // -1
    kprintf("%d\n", 2147483647);                        // 2147483647
    kprintf("%d\n", -2147483647 - 1);                   // -2147483648
    kprintf("%x\n", 0u);                                // 0
    kprintf("%x\n", 255u);                              // ff
    kprintf("%x\n", 0xDEADBEEFu);                       // deadbeef
    kprintf("%p\n", reinterpret_cast<void *>(0x1000));  // 0x1000

    // TODO: once %lu, %lx and %ld exist, add these:
    //   uint64_t max   18446744073709551615   ->  18446744073709551615
    //   uint64_t max   %lx                    ->  ffffffffffffffff
    //   INT64_MIN      -9223372036854775808   ->  -9223372036854775808

    kprintf("--- end selftest ---\n");
}
