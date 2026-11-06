// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#pragma once

#include <stdint.h>

namespace kprintf_detail {

    // Print |value| in |base| (2..16), lowercase digits, no prefix, no padding.
    void put_uint(uint64_t value, unsigned base);

    // Print |value| in base 10 with a leading '-' if negative.
    void put_int(int64_t value);

}  // namespace kprintf_detail

// Supported conversions: %c %s %d %u %x %p %%
// The format attribute makes the compiler check arguments against the format string.
__attribute__((format(printf, 1, 2))) void kprintf(const char *fmt, ...);
