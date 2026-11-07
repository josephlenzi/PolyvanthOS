// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include "serial.hpp"

#include <stdint.h>

#include "arch/x86_64/port_io.hpp"

namespace serial {
    namespace {
        constexpr uint16_t COM1_BASE = 0x3F8;

        // Register offsets from the base port.
        constexpr uint16_t UART_DATA = 0;  // TX/RX byte; divisor low byte when DLAB=1
        constexpr uint16_t UART_IER  = 1;  // Interrupt enable; divisor high byte when DLAB=1
        constexpr uint16_t UART_FCR  = 2;  // FIFO control (write-only)
        constexpr uint16_t UART_LCR  = 3;  // Line control
        constexpr uint16_t UART_MCR  = 4;  // Modem control
        constexpr uint16_t UART_LSR  = 5;  // Line status

        // Line control register bits.
        constexpr uint8_t LCR_8N1  = 0x03;  // 8 data bits, no parity, 1 stop bit
        constexpr uint8_t LCR_DLAB = 0x80;  // Divisor latch access: remaps +0/+1 to the divisor

        // FIFO control: enable, clear both FIFOs, 14-byte receive trigger.
        constexpr uint8_t FCR_ENABLE_CLEAR_14 = 0xC7;

        // Modem control: DTR + RTS + OUT2.
        constexpr uint8_t MCR_READY = 0x0B;

        // Line status register bits.
        constexpr uint8_t LSR_TX_EMPTY = 0x20;  // Transmit holding register can accept a byte

        constexpr uint32_t UART_BASE_BAUD = 115200;  // UART clock
        constexpr int TX_TIMEOUT          = 100000;  // Timeout for putc busy-wait
    }  // namespace

    void init(uint32_t baud_rate) {
        if (baud_rate == 0 || baud_rate > UART_BASE_BAUD) {
            baud_rate = DEFAULT_BAUD;  // Can't panic here: panic prints through this driver
        }
        const uint16_t divisor = static_cast<uint16_t>(UART_BASE_BAUD / baud_rate);

        arch::outb(COM1_BASE + UART_IER, 0x00);  // polled mode: no UART interrupts

        arch::outb(COM1_BASE + UART_LCR, LCR_DLAB);  // expose the divisor registers
        arch::outb(COM1_BASE + UART_DATA, static_cast<uint8_t>(divisor & 0xFF));
        arch::outb(COM1_BASE + UART_IER,
                   static_cast<uint8_t>(divisor >> 8));  // with DLAB=1, +1 is the divisor high byte

        arch::outb(COM1_BASE + UART_LCR, LCR_8N1);  // sets the format and clears DLAB
        arch::outb(COM1_BASE + UART_FCR, FCR_ENABLE_CLEAR_14);
        arch::outb(COM1_BASE + UART_MCR, MCR_READY);
    }

    void putc(char c) {
        int timeout = TX_TIMEOUT;
        while ((arch::inb(COM1_BASE + UART_LSR) & LSR_TX_EMPTY) == 0 && --timeout > 0) {
            // busy-wait until the transmitter can take another byte, or timeout
        }
        if (timeout > 0) {
            arch::outb(COM1_BASE + UART_DATA, static_cast<uint8_t>(c));
        }
        // On timeout the character is dropped.
    }

    void puts(const char *s) {
        while (*s) {
            if (*s == '\n') {
                putc('\r');
            }
            putc(*s++);
        }
    }

}  // namespace serial
