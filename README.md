<p align="center">
  <img src="docs/logo.svg" width="128" height="128" alt="PolyvanthOS logo">
</p>

# PolyvanthOS

A 64-bit multitasking hobby OS with native, MS-DOS, and Linux personalities.

## Build

    sudo apt install clang lld make git xorriso qemu-system-x86
    git clone --recurse-submodules <repo-url>
    cd polyvanthos
    make -C limine
    make run

Output appears on the serial console. Press Ctrl+A then X to quit QEMU.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) and the [Code of Conduct](CODE_OF_CONDUCT.md). To report a
security issue, see [SECURITY.md](SECURITY.md).

## Authors

See [AUTHORS](AUTHORS).

## License

GPL-2.0-only. See LICENSE. Limine is BSD-2-Clause and keeps its own license.

MS-DOS is a trademark of Microsoft Corporation. Linux is a registered trademark of Linus
Torvalds. PolyvanthOS is not affiliated with or endorsed by either.

## AI assistance

Parts of this project were developed with help from Claude AI and GitHub Copilot.
