// SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
// SPDX-License-Identifier: GPL-2.0-only
#pragma once

// Print a message and stop the kernel. Never returns.
[[noreturn]] void panic(const char *message);
