// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include "idt.hpp"

#include <stdint.h>

#include "gdt.hpp"
#include "lib/kprintf.hpp"
#include "lib/panic.hpp"

// Stub addresses, defined in isr.S. extern "C" so the name matches the asm one.
extern "C" const uint64_t isr_stub_table[32];

namespace arch {
    namespace {

        // One IDT entry (a "gate"). The handler address is split into 3 pieces.
        struct __attribute__((packed)) IdtEntry {
            uint16_t offset_low;   // address bits 0-15
            uint16_t selector;     // code segment the handler runs in
            uint8_t ist;           // special stack number, 0 = none (needs a TSS)
            uint8_t type_attr;     // gate type and flags
            uint16_t offset_mid;   // address bits 16-31
            uint32_t offset_high;  // address bits 32-63
            uint32_t reserved;     // must be 0
        };

        // 256 vectors: 0-31 are CPU exceptions, the rest are for hardware interrupts later.
        // Unused entries stay zero, which means "not present".
        alignas(16) IdtEntry idt[256] = {};

        // What lidt reads: table size minus 1, then its address
        struct __attribute__((packed)) Idtr {
            uint16_t limit;
            uint64_t base;
        };

        void set_gate(int vector, uint64_t handler) {
            idt[vector].offset_low  = handler & 0xFFFF;
            idt[vector].selector    = KERNEL_CODE_SELECTOR;
            idt[vector].ist         = 0;
            idt[vector].type_attr   = 0x8E;  // present, ring 0, 64-bit interrupt gate
            idt[vector].offset_mid  = (handler >> 16) & 0xFFFF;
            idt[vector].offset_high = handler >> 32;
            idt[vector].reserved    = 0;
        }

        const char *const exception_names[32] = {
            "Divide error",
            "Debug",
            "NMI",
            "Breakpoint",
            "Overflow",
            "Bound range exceeded",
            "Invalid opcode",
            "Device not available",
            "Double fault",
            "Coprocessor segment overrun",
            "Invalid TSS",
            "Segment not present",
            "Stack-segment fault",
            "General protection fault",
            "Page fault",
            "Reserved",
            "x87 floating-point exception",
            "Alignment check",
            "Machine check",
            "SIMD floating-point exception",
            "Virtualization exception",
            "Control protection exception",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Hypervisor injection exception",
            "VMM communication exception",
            "Security exception",
            "Reserved",
        };

    }  // namespace

    void idt_init() {
        for (int i = 0; i < 32; ++i) {
            set_gate(i, isr_stub_table[i]);
        }

        Idtr idtr = {sizeof(idt) - 1, reinterpret_cast<uint64_t>(idt)};
        __asm__ volatile("lidt %0" : : "m"(idtr) : "memory");
    }

    // Called by isr_common in isr.S. extern "C" so the asm can find it by name.
    // Exceptions don't return yet, so nothing is saved or restored.
    extern "C" [[noreturn]] void exception_handler(uint64_t vector, uint64_t error_code) {
        const char *name = vector < 32 ? exception_names[vector] : "Unknown";
        kprintf("CPU exception %lu: %s (error code 0x%lx)\n", vector, name, error_code);
        panic("CPU exception");
    }

}  // namespace arch
