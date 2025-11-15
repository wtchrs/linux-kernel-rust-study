# Linux kernel with rust modules

## Submodules

```bash
git submodule update --init --recursive --depth=1
```

## Linux kernel

```bash
./make-linux.sh rustavailable
./make-linux.sh menuconfig
# Set `General setup > Rust support`
# Save and exit
./make-linux.sh
```

## BusyBox

```bash
./make-busybox.sh menuconfig
# Set `Settings > Build static binary (no shared libs)`
# If you are in Arch Linux, Unset `Networking Utilities > tc`
# Save and exit
./make-busybox.sh install
```

If you have some ncurses problem, apply patch with the following command:

```bash
git apply busybox-ncurses.patch
```

## Make initramfs image

```bash
mkdir -p initramfs/etc initramfs/proc initramfs/sys

cat << 'EOF' > initramfs/init
#!/bin/sh
/bin/mount -t devtmpfs devtmpfs /dev
/bin/mount -t proc none /proc
/bin/mount -t sysfs none /sys
exec 0</dev/console
exec 1>/dev/console
exec 2>/dev/console
exec /bin/sh
EOF

chmod a+x initramfs/init
sudo chown -hR root:root initramfs

cd initramfs
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz
```

You can find the initramfs image in the `initramfs.cpio.gz` file.

## Launch kernel image using qemu

```bash
./qemu.sh
```

## CREDITS

- BusyBox ncurses patch from https://stackoverflow.com/a/78545186
