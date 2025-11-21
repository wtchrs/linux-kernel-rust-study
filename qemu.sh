#!/usr/bin/env bash

if ! [ -d kernel_module ]; then
    mkdir kernel_module
fi

qemu-system-x86_64 \
    -kernel linux-build/arch/x86/boot/bzImage \
    -initrd initramfs.cpio.gz \
    -nographic \
    -append "console=ttyS0" \
    -virtfs local,path=kernel_module,security_model=none,mount_tag=kernel_modules
