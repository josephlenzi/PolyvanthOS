// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#pragma once

namespace arch {

    // Load the IDT so CPU exceptions (divide by zero, page fault, ...) reach our handler.
    void idt_init();

}  // namespace arch
