// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <stdint.h>

namespace serial {

    constexpr uint32_t DEFAULT_BAUD = 115200;

    void init(uint32_t baud_rate);
    void putc(char c);
    void puts(const char *s);

}  // namespace serial
