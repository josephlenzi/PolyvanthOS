// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include <stdint.h>

#include "limine.h"
#include "serial.hpp"

// clang-format off
__attribute__((used, section(".data.limine")))
static volatile uint64_t limine_base_revision[3] =
    LIMINE_BASE_REVISION(3);
// clang-format on

extern "C" [[noreturn]] void kmain() {
    serial::init(serial::DEFAULT_BAUD);
    serial::puts("PolyvanthOS booting...\n");
    for (;;) {
        __asm__ volatile("hlt");
    }
}
