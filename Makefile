# SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
# SPDX-License-Identifier: GPL-2.0-only

CXX      = clang++
CXXFLAGS = --target=x86_64-unknown-none-elf -std=c++20 -O2 -g -Wall -Wextra \
           -ffreestanding -fno-exceptions -fno-rtti -fno-threadsafe-statics \
           -fno-use-cxa-atexit -fno-stack-protector -fno-pic \
           -mno-red-zone -mcmodel=kernel -mno-sse -mno-mmx -Isrc

SRCS     = src/kmain.cpp src/selftest.cpp src/drivers/serial.cpp src/lib/kprintf.cpp \
           src/lib/panic.cpp src/arch/x86_64/gdt.cpp \
           src/arch/x86_64/idt.cpp
ASMS     = src/arch/x86_64/isr.S
OBJS     = $(SRCS:src/%.cpp=build/%.o) $(ASMS:src/%.S=build/%.o)
LINKER   = src/arch/x86_64/linker.ld
ISO      = polyvanthos.iso

all: $(ISO)

kernel: $(OBJS) $(LINKER)
	ld.lld -nostdlib -static -z max-page-size=0x1000 -T $(LINKER) -o $@ $(OBJS)

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: src/%.S
	@mkdir -p $(@D)
	$(CXX) --target=x86_64-unknown-none-elf -c $< -o $@

$(ISO): kernel limine.conf
	mkdir -p iso/boot/limine iso/EFI/BOOT
	cp kernel iso/boot/
	cp limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin \
	   limine/limine-uefi-cd.bin iso/boot/limine/
	cp limine/BOOTX64.EFI iso/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
	  -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
	  -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
	  -efi-boot-part --efi-boot-image --protective-msdos-label iso -o $@
	./limine/limine bios-install $@

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -serial stdio -display none

clean:
	rm -rf build kernel iso $(ISO)

# Formatting and licensing checks
FORMAT_FILES = $(shell find src -name '*.cpp' -o -name '*.hpp' | grep -v '^src/boot/')

check:
	clang-format --dry-run --Werror $(FORMAT_FILES)
	reuse lint

# Boot in QEMU and check the selftest finishes without a panic
test: $(ISO)
	@mkdir -p build
	@rm -f build/serial.log
	-timeout 15 qemu-system-x86_64 -cdrom $(ISO) -serial file:build/serial.log -display none -no-reboot
	@if grep -q -- "--- end selftest ---" build/serial.log && ! grep -q "PANIC" build/serial.log; then \
		echo "selftest passed"; \
	else \
		cat build/serial.log; echo "selftest FAILED"; exit 1; \
	fi

.PHONY: all run clean check test
