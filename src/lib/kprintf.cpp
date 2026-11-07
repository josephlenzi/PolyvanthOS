// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#include "kprintf.hpp"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "drivers/serial.hpp"

namespace {
    constexpr char DIGIT_TABLE[] = "0123456789abcdef";

    // All kprintf output goes through here, so a framebuffer or log buffer can be added later.
    void emit(char c) {
        if (c == '\n') {
            serial::putc('\r');  // serial terminals need CR+LF
        }
        serial::putc(c);
    }

    // Pull the next argument based on the number of 'l's
    int64_t pull_signed(va_list &args, int long_count) {
        if (long_count == 0) {
            return va_arg(args, int);
        }
        if (long_count == 1) {
            return va_arg(args, long);
        }
        return va_arg(args, long long);
    }

    uint64_t pull_unsigned(va_list &args, int long_count) {
        if (long_count == 0) {
            return va_arg(args, unsigned int);
        }
        if (long_count == 1) {
            return va_arg(args, unsigned long);
        }
        return va_arg(args, unsigned long long);
    }

}  // namespace

namespace kprintf_detail {

    void put_uint(uint64_t value, unsigned base) {
        if (base < 2 || base > 16) {
            base = 10;  // TODO: assert once panic exists
        }

        char buffer[64];
        size_t counter = 0;

        // Extract digits
        do {
            uint64_t remainder = value % base;
            buffer[counter++]  = DIGIT_TABLE[remainder];
            value /= base;
        } while (value != 0);

        // Print in reverse
        for (size_t i = counter; i > 0; --i) {
            emit(buffer[i - 1]);
        }
    }

    void put_int(int64_t value) {
        if (value < 0) {
            emit('-');
        }

        // Negate in unsigned arithmetic: well-defined even for INT64_MIN.
        uint64_t magnitude = static_cast<uint64_t>(value);
        if (value < 0) {
            magnitude = 0 - magnitude;
        }
        put_uint(magnitude, 10);
    }

}  // namespace kprintf_detail

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; ++p) {
        char c = *p;

        if (c == '%') {
            // Handle conversion
            ++p;  // Move to the conversion letter
            if (*p == '\0') {
                break;  // End of string after '%', stop cleanly
            }

            // Count the 'l' characters
            int long_count = 0;
            while (*p == 'l') {
                ++long_count;
                ++p;
            }

            if (*p == '\0') {
                break;  // End of string after "%l", stop cleanly
            }

            char conversion = *p;
            switch (conversion) {
            case 'c': {
                int arg = va_arg(args, int);
                emit(static_cast<char>(arg));
                break;
            }
            case 's': {
                const char *arg = va_arg(args, const char *);
                if (arg == nullptr) {
                    arg = "(null)";
                }
                while (*arg != '\0') {
                    emit(*arg++);
                }
                break;
            }
            case 'd': {
                kprintf_detail::put_int(pull_signed(args, long_count));
                break;
            }
            case 'u': {
                kprintf_detail::put_uint(pull_unsigned(args, long_count), 10);
                break;
            }
            case 'x': {
                kprintf_detail::put_uint(pull_unsigned(args, long_count), 16);
                break;
            }
            case 'p': {
                void *arg = va_arg(args, void *);
                emit('0');
                emit('x');
                kprintf_detail::put_uint(reinterpret_cast<uintptr_t>(arg), 16);
                break;
            }
            case '%': {
                emit('%');
                break;
            }
            default: {
                emit('%');
                emit(conversion);
                break;
            }
            }
        } else {
            // Ordinary character
            emit(c);
        }
    }

    va_end(args);
}
