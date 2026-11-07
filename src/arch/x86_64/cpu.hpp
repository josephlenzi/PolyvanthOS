// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#pragma once

namespace arch {

    // Stop this CPU for good.
    [[noreturn]] inline void halt_forever() {
        __asm__ volatile("cli");  // No interrupts after this

        // An NMI can still wake hlt, so loop back to sleep
        for (;;) {
            __asm__ volatile("hlt");
        }
    }

}  // namespace arch
