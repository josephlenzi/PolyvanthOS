// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include <stdint.h>

#include "arch/x86_64/cpu.hpp"
#include "arch/x86_64/gdt.hpp"
#include "boot/limine.h"
#include "drivers/serial.hpp"
#include "lib/panic.hpp"
#include "selftest.hpp"

// Limine scans for these in order: start marker, requests, end marker.
// The linker script keeps them together.
// clang-format off
__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[3] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end[] = LIMINE_REQUESTS_END_MARKER;
// clang-format on

extern "C" [[noreturn]] void kmain() {
    serial::init(serial::DEFAULT_BAUD);
    serial::puts("PolyvanthOS booting...\n");

    // Check Limine accepted our base revision. Needs serial, so it can print.
    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        panic("Limine base revision not supported");
    }

    arch::gdt_init();

    kprintf_selftest();

    arch::halt_forever();
}
