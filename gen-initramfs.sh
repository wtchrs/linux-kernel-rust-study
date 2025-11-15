#!/usr/bin/env bash
set -euo pipefail

CWD=$(pwd)
INITRAMFS_DIR=./initramfs

mkdir -p "${INITRAMFS_DIR}"/{etc,proc,sys,mnt,rust_modules}

cat <<'EOF' >"${INITRAMFS_DIR}/init"
#!/bin/sh
/bin/mount -t devtmpfs devtmpfs /dev
/bin/mount -t proc none /proc
/bin/mount -t sysfs none /sys
/bin/mount -t 9p -o trans=virtio rust_modules /rust_modules
exec 0</dev/console
exec 1>/dev/console
exec 2>/dev/console
exec /bin/sh
EOF

chmod a+x "${INITRAMFS_DIR}/init"

cd ${INITRAMFS_DIR}

find . -print0 |
    cpio --null -ov --format=newc --owner=0:0 |
    gzip -9 >"${CWD}/initramfs.cpio.gz"
