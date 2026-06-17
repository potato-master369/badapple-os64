#!/bin/bash

qemu-system-x86_64 -m 2G -enable-kvm  -smp 2   -drive if=pflash,format=raw,readonly=on,file=OVMF_CODE_4M.fd   -drive if=pflash,format=raw,file=./OVMF_VARS_4M.fd   -drive file=fat:rw:esp_test,format=raw,media=disk
