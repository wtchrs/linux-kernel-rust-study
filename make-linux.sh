#!/usr/bin/env bash

make LLVM=1 O=../linux-build -j$(nproc) -C linux $@
