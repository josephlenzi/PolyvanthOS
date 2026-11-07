// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include "gdt.hpp"

#include <stdint.h>

namespace arch {
    namespace {

        // Entry 0 is null (required), 1 is kernel code, 2 is kernel data.
        // Entry N is at byte offset N * 8, which is where the selectors come from.
        alignas(8) uint64_t gdt[3] = {0, 0, 0};

        // What lgdt reads: table size minus 1, then its address. Packed so there's no padding.
        struct __attribute__((packed)) Gdtr {
            uint16_t limit;
            uint64_t base;
        };

    }  // namespace

    void gdt_init() {
        // Base and limit are ignored in 64-bit mode, so only the access byte (bits 40-47)
        // and flags (bits 52-55) are set.
        // 0x9A = present, ring 0, code, executable, readable. Bit 53 (L) = 64-bit code.
        // 0x92 = present, ring 0, data, writable.
        gdt[1] = (uint64_t{0x9A} << 40) | (uint64_t{1} << 53);
        gdt[2] = (uint64_t{0x92} << 40);

        // Set up here, not at file scope, so it never depends on a startup constructor.
        Gdtr gdtr = {sizeof(gdt) - 1, reinterpret_cast<uint64_t>(gdt)};
        __asm__ volatile("lgdt %0" : : "m"(gdtr) : "memory");

        // lgdt doesn't touch the segment registers, so reload them from the new table
        __asm__ volatile("mov %w0, %%ds\n"
                         "mov %w0, %%es\n"
                         "mov %w0, %%fs\n"
                         "mov %w0, %%gs\n"
                         "mov %w0, %%ss\n"
                         :
                         : "r"(KERNEL_DATA_SELECTOR)
                         : "memory");

        // cs can't be set with mov. Push the new cs and the address of label 1, then far
        // return, which pops both.
        __asm__ volatile("pushq %0\n"
                         "leaq 1f(%%rip), %%rax\n"
                         "pushq %%rax\n"
                         "lretq\n"
                         "1:\n"
                         :
                         : "i"(KERNEL_CODE_SELECTOR)
                         : "rax", "memory");
    }

}  // namespace arch
