// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include "panic.hpp"

#include "arch/x86_64/cpu.hpp"
#include "lib/kprintf.hpp"

void panic(const char *message) {
    // Stop here if printing the message panics too
    static bool already_panicking = false;
    if (already_panicking) {
        arch::halt_forever();
    }
    already_panicking = true;

    // Use %s so a % in the message is not read as a format
    kprintf("PANIC: %s\n", message);

    arch::halt_forever();
}
