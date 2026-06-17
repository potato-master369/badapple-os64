#!/bin/bash

#-------------------------------------#
# script to compile                   #
#-------------------------------------#

gcc -Ignu-efi-dir/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -O3 -I/usr/include/efi -I/usr/include/efi/x86_64 -c src/efi.c -o main.o
ld -shared -Bsymbolic -Lgnu-efi-dir/x86_64/lib -L/usr/lib -T/usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o main.o -o main.so -lgnuefi -lefi
objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target efi-app-x86_64 --subsystem=10 main.so main.efi

