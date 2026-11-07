// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#pragma once

#include <stdint.h>

namespace arch {

    // Selectors: the byte offset of each entry in the GDT
    constexpr uint16_t KERNEL_CODE_SELECTOR = 0x08;  // entry 1
    constexpr uint16_t KERNEL_DATA_SELECTOR = 0x10;  // entry 2

    // Load our own GDT, replacing Limine's
    void gdt_init();

}  // namespace arch
