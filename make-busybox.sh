#!/usr/bin/env bash

if ! [ -d ./busybox-build ]; then
    mkdir -p busybox-build
fi

make LLVM=1 O=../busybox-build CONFIG_PREFIX=../initramfs -j$(nproc) -C busybox $@
