#!/bin/bash

export PREFIX="/mnt/c/kernel/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

rm *.o
rm os.bin
rm -r -f isodir

nasm -felf32 start.asm -o start.o
i686-elf-gcc -c main.c -o main.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c irq.c -o irq.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c gdt.c -o gdt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c idt.c -o idt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c isrs.c -o isrs.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c kb.c -o kb.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c timer.c -o timer.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c scrn.c -o scrn.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c pcspeaker.c -o pcspeaker.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -c systemtime.c -o systemtime.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -w
i686-elf-gcc -T linker.ld -o os.bin -ffreestanding -O2 -nostdlib start.o main.o isrs.o irq.o gdt.o idt.o timer.o scrn.o kb.o pcspeaker.o systemtime.o -lgcc
grub-file --is-x86-multiboot os.bin
if grub-file --is-x86-multiboot os.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi
rm -r isodir
mkdir -p isodir/boot/grub
cp os.bin isodir/boot/os.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o os.iso isodir --xorriso=./xorriso/xorriso-1.4.6/xorriso/xorriso

rm *.o
rm -r -f isodir
