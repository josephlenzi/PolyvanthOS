# PolyvanthOS

A 64-bit multitasking hobby OS with native, MS-DOS, and Linux personalities.

## Build

    sudo apt install clang lld make git xorriso qemu-system-x86
    git clone --recurse-submodules <repo-url>
    cd polyvanthos
    make -C limine
    make run

Output appears on the serial console. Press Ctrl+A then X to quit QEMU.
## License

GPL-2.0-only. See LICENSE. Limine is BSD-2-Clause and keeps its own license.
