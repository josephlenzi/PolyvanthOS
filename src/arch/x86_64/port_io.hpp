// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <stdint.h>

// x86 port I/O. Arch-specific: other architectures provide their own
// equivalent (e.g. MMIO on ARM) behind the same interface.
namespace arch {

    inline void outb(uint16_t port, uint8_t val) {
        __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
    }

    inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
        return ret;
    }

}  // namespace arch
