# SPDX-FileCopyrightText: 2026 PolyvanthOS contributors
# SPDX-License-Identifier: GPL-2.0-only

CXX      = clang++
CXXFLAGS = --target=x86_64-unknown-none-elf -std=c++20 -O2 -g -Wall -Wextra \
           -ffreestanding -fno-exceptions -fno-rtti -fno-threadsafe-statics \
           -fno-use-cxa-atexit -fno-stack-protector -fno-pic \
           -mno-red-zone -mcmodel=kernel -mno-sse -mno-mmx -I.
OBJS     = kmain.o serial.o
ISO    = polyvanthos.iso

all: $(ISO)

kernel: $(OBJS) linker.ld
	ld.lld -nostdlib -static -z max-page-size=0x1000 -T linker.ld -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

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
	rm -rf *.o kernel iso $(ISO)